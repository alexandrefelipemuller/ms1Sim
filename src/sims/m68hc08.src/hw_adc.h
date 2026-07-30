/*
 * Simulator of microcontrollers (m68hc08.src/hw_adc.h)
 *
 * Minimal ADC stub for the 68HC908GP32 register map: completes every
 * conversion on the next tick with a fixed mid-scale reading, just enough
 * to unblock firmware boot code that waits on COCO before touching serial.
 */

#ifndef HW_ADC_HEADER
#define HW_ADC_HEADER

#include "hwcl.h"

enum adc_reg_idx
  {
   adc_adscr = 0, // $3C ADC Status and Control Register
   adc_adr   = 1, // $3D ADC Data Register
   adc_adclk = 2, // $3E ADC Clock Register
  };

class cl_adc: public cl_hw
{
 protected:
  bool conversion_pending;
 public:
  cl_adc(class cl_uc *auc, int aid, t_addr abase);
  virtual int init(void);
  t_addr base;
  class cl_memory_cell *regs[3];

  virtual t_mem read(class cl_memory_cell *cell);
  virtual void write(class cl_memory_cell *cell, t_mem *val);
  virtual int tick(int cycles);
};

#endif

/* End of m68hc08.src/hw_adc.h */
