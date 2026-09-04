/*
 * Simulator of microcontrollers (m68hc08.src/hw_gpio_observer.h)
 *
 * Watches PTA/PTB (GP32 port data registers) and logs any bit transition,
 * timestamped in simulated microseconds, so ignition/injection pulses
 * driven by firmware can be observed without a scope.
 */

#ifndef HW_GPIO_OBSERVER_HEADER
#define HW_GPIO_OBSERVER_HEADER

#include "hwcl.h"

#define PTA_ADDR ((t_addr)0x0000)
#define PTB_ADDR ((t_addr)0x0001)

class cl_gpio_observer: public cl_hw
{
protected:
  FILE *log_f;
  bool own_log_f;

  u8_t last_pta;
  u8_t last_ptb;

  class cl_memory_cell *pta;
  class cl_memory_cell *ptb;

  double total_cycles;

  void log_transitions(const char *port_name, u8_t before, u8_t after);

public:
  cl_gpio_observer(class cl_uc *auc, int aid);
  virtual int init(void);
  virtual void write(class cl_memory_cell *cell, t_mem *val);
  virtual t_mem read(class cl_memory_cell *cell);
  virtual int tick(int cycles);
};

#endif

/* End of m68hc08.src/hw_gpio_observer.h */
