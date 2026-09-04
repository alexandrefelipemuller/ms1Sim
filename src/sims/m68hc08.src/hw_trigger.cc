/*
 * Simulator of microcontrollers (m68hc08.src/hw_trigger.cc)
 *
 * Crank-wheel trigger generator for the GP32 IRQ pin.
 */

#include <stdlib.h>
#include <string.h>

#include "m68hc08cl.h"
#include "hw_trigger.h"

#define VEC_IRQ ((t_addr)0xFFFA)


cl_trigger::cl_trigger(class cl_uc *auc, int aid):
  cl_hw(auc, HW_DUMMY, aid, "trigger")
{
  enabled= false;
  rpm= 0;
  teeth_total= 36;
  teeth_missing= 1;
  cur_tooth= 0;
  cycles_per_tooth= 0;
  cycle_accum= 0;
  intscr= 0;
}

void
cl_trigger::parse_pattern(const char *pat)
{
  if (!pat || !*pat)
    return;
  int total= 0, missing= 0;
  if (sscanf(pat, "%d-%d", &total, &missing) == 2 && total > 0 && missing >= 0 && missing < total)
    {
      teeth_total= total;
      teeth_missing= missing;
    }
}

int
cl_trigger::init(void)
{
  // configuration comes from the environment (set by the ucsim_ms1
  // wrapper script) rather than ucsim's -S/option mechanism, because
  // hw::init() runs before any -e startup command / generic option
  // machinery would have a chance to set values for a fresh option
  const char *rpm_env= getenv("MS1_TRIGGER_RPM");
  const char *pattern_env= getenv("MS1_TRIGGER_PATTERN");
  const char *enable_env= getenv("MS1_TRIGGER_ENABLE");

  if (rpm_env && *rpm_env)
    rpm= atof(rpm_env);
  if (pattern_env && *pattern_env)
    parse_pattern(pattern_env);

  bool en= (rpm > 0);
  if (enable_env && *enable_env)
    en= (atoi(enable_env) != 0);
  enabled= en && (rpm > 0);

  intscr_addr= INTSCR_ADDR;
  intscr= register_cell(uc->rom, intscr_addr);

  if (enabled)
    {
      double teeth_per_rev= teeth_total; // full wheel period always spans teeth_total slots
      double revs_per_sec= rpm / 60.0;
      double secs_per_tooth= 1.0 / (revs_per_sec * teeth_per_rev);
      cycles_per_tooth= secs_per_tooth / uc->get_xtal_tick();
    }

  return(0);
}

int
cl_trigger::tick(int cycles)
{
  if (!enabled || cycles_per_tooth <= 0)
    return 0;

  cycle_accum+= cycles;
  while (cycle_accum >= cycles_per_tooth)
    {
      cycle_accum-= cycles_per_tooth;
      cur_tooth= (cur_tooth + 1) % teeth_total;

      // skip the missing-tooth gap(s): no pulse for those tooth slots
      bool in_gap= cur_tooth < teeth_missing;
      if (!in_gap)
	{
	  // pulse the IRQ pin: set IRQF, then request the IRQ vector
	  u32_t r= intscr->get();
	  r|= (1 << INTSCR_IRQF);
	  intscr->set(r);
	  ((cl_hc08*)uc)->request_irq(VEC_IRQ);
	}
    }
  return 0;
}

/* End of m68hc08.src/hw_trigger.cc */
