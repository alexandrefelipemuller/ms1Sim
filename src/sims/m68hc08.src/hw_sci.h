/*
 * Simulator of microcontrollers (m68hc08.src/hw_sci.h)
 *
 * Minimal SCI (serial) peripheral model for the 68HC908GP32 register map,
 * used to let interrupt-driven firmware (e.g. MegaSquirt MS1) talk to the
 * outside world over the -S port=/in=/out= serial transport.
 */

#ifndef HW_SCI_HEADER
#define HW_SCI_HEADER

#include "serial_hwcl.h"

enum sci_reg_idx
  {
   sci_scc1 = 0, // $13 SCI Control Register 1
   sci_scc2 = 1, // $14 SCI Control Register 2
   sci_scc3 = 2, // $15 SCI Control Register 3
   sci_scs1 = 3, // $16 SCI Status Register 1
   sci_scs2 = 4, // $17 SCI Status Register 2
   sci_scdr = 5, // $18 SCI Data Register
   sci_scbr = 6, // $19 SCI Baud Rate Register
  };

class cl_sci: public cl_serial_hw
{
 protected:
  bool tx_busy;
 public:
  cl_sci(class cl_uc *auc, int aid, t_addr abase);
  virtual int init(void);
  virtual int dev_size(void) { return 7; }

  virtual t_mem read(class cl_memory_cell *cell);
  virtual void write(class cl_memory_cell *cell, t_mem *val);
  virtual int tick(int cycles);
};

#endif

/* End of m68hc08.src/hw_sci.h */
