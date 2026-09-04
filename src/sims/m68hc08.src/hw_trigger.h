/*
 * Simulator of microcontrollers (m68hc08.src/hw_trigger.h)
 *
 * Crank-wheel trigger generator: fires IRQ pulses on a missing-tooth
 * wheel pattern (e.g. 36-1) at a configurable RPM, to drive firmware
 * trigger decoders (see Slowduino-HC08 decoders.c) without real hardware.
 */

#ifndef HW_TRIGGER_HEADER
#define HW_TRIGGER_HEADER

#include "hwcl.h"

#define INTSCR_ADDR ((t_addr)0x001D)
#define INTSCR_IRQF 3

class cl_trigger: public cl_hw
{
protected:
  bool enabled;
  double rpm;
  int teeth_total;
  int teeth_missing;

  int cur_tooth;
  t_addr intscr_addr;
  class cl_memory_cell *intscr;

  double cycles_per_tooth;
  double cycle_accum;

  void parse_pattern(const char *pat);

public:
  cl_trigger(class cl_uc *auc, int aid);
  virtual int init(void);
  virtual int tick(int cycles);
};

#endif

/* End of m68hc08.src/hw_trigger.h */
