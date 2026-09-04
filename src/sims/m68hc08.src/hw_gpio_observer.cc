/*
 * Simulator of microcontrollers (m68hc08.src/hw_gpio_observer.cc)
 *
 * Logs bit transitions on PTA/PTB (ignition/injection outputs on the
 * Slowduino-HC08 port, see scheduler.c) so pulses can be observed
 * without a scope.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "m68hc08cl.h"
#include "hw_gpio_observer.h"


cl_gpio_observer::cl_gpio_observer(class cl_uc *auc, int aid):
  cl_hw(auc, HW_DUMMY, aid, "gpio_observer")
{
  log_f= NULL;
  own_log_f= false;
  last_pta= 0;
  last_ptb= 0;
  total_cycles= 0;
  pta= ptb= NULL;
}

int
cl_gpio_observer::init(void)
{
  const char *path= getenv("MS1_GPIO_LOG");
  if (path && *path)
    {
      log_f= fopen(path, "w");
      own_log_f= (log_f != NULL);
    }
  if (!log_f)
    log_f= stdout;

  pta= register_cell(uc->rom, PTA_ADDR);
  ptb= register_cell(uc->rom, PTB_ADDR);
  last_pta= pta->get();
  last_ptb= ptb->get();

  return(0);
}

void
cl_gpio_observer::log_transitions(const char *port_name, u8_t before, u8_t after)
{
  u8_t changed= before ^ after;
  if (!changed)
    return;
  double us= total_cycles * uc->get_xtal_tick() * 1e6;
  for (int bit= 0; bit < 8; bit++)
    {
      u8_t mask= (1 << bit);
      if (changed & mask)
	{
	  fprintf(log_f, "[%12.2f us] %s bit%d %s\n",
		  us, port_name, bit,
		  (after & mask) ? "HIGH" : "LOW");
	}
    }
  fflush(log_f);
}

void
cl_gpio_observer::write(class cl_memory_cell *cell, t_mem *val)
{
  if (cell == pta)
    {
      u8_t before= last_pta;
      cell->set(*val);
      u8_t after= (u8_t)cell->get();
      log_transitions("PTA", before, after);
      last_pta= after;
    }
  else if (cell == ptb)
    {
      u8_t before= last_ptb;
      cell->set(*val);
      u8_t after= (u8_t)cell->get();
      log_transitions("PTB", before, after);
      last_ptb= after;
    }
  else
    cell->set(*val);
}

t_mem
cl_gpio_observer::read(class cl_memory_cell *cell)
{
  return cell->get();
}

int
cl_gpio_observer::tick(int cycles)
{
  total_cycles+= cycles;
  return 0;
}

/* End of m68hc08.src/hw_gpio_observer.cc */
