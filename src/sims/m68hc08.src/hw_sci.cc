/*
 * Simulator of microcontrollers (m68hc08.src/hw_sci.cc)
 *
 * Minimal SCI (serial) peripheral model for the 68HC908GP32 register map,
 * used to let interrupt-driven firmware (e.g. MegaSquirt MS1) talk to the
 * outside world over the -S port=/in=/out= serial transport.
 */

#include "m68hc08cl.h"
#include "hw_sci.h"

// bit positions per Gp32.equ
#define SCI_SCTIE 0x80 // scc2: transmit interrupt enable
#define SCI_SCRIE 0x20 // scc2: receive interrupt enable
#define SCI_SCTE  0x80 // scs1: transmitter empty
#define SCI_SCRF  0x20 // scs1: receiver full

#define VEC_SCITX ((t_addr)0xFFE2)
#define VEC_SCIRX ((t_addr)0xFFE4)


cl_sci::cl_sci(class cl_uc *auc, int aid, t_addr abase):
  cl_serial_hw(auc, aid, "sci")
{
  base= abase;
  tx_busy= false;
}

int
cl_sci::init(void)
{
  cl_serial_hw::init();
  cfg_set(serconf_check_often, 1);
  map(uc->rom, base);
  regs[sci_scs1]->set(SCI_SCTE);
  return(0);
}

t_mem
cl_sci::read(class cl_memory_cell *cell)
{
  if (cell == regs[sci_scdr])
    {
      u32_t r= regs[sci_scs1]->get();
      r&= ~SCI_SCRF;
      regs[sci_scs1]->set(r);
      return regs[sci_scdr]->get();
    }
  return cell->get();
}

void
cl_sci::write(class cl_memory_cell *cell, t_mem *val)
{
  cell->set(*val);
  if (cell == regs[sci_scdr])
    {
      u8_t b= (*val) & 0xff;
      io->write((char*)&b, 1);
      // real hardware clears SCTE while the byte shifts out; the TX
      // interrupt fires again only once SCTE re-asserts, one tick later,
      // which paces back-to-back sends instead of re-entering instantly
      u32_t r= regs[sci_scs1]->get();
      r&= ~SCI_SCTE;
      regs[sci_scs1]->set(r);
      tx_busy= true;
    }
  else if (cell == regs[sci_scc2])
    {
      // real SCI hardware fires the TX interrupt as soon as SCTIE is set
      // while SCTE is already true, not only on the next SCDR write
      if ((*val & SCI_SCTIE) && (regs[sci_scs1]->get() & SCI_SCTE))
	((cl_hc08*)uc)->request_irq(VEC_SCITX);
    }
}

int
cl_sci::tick(int cycles)
{
  if (tx_busy)
    {
      tx_busy= false;
      u32_t r= regs[sci_scs1]->get();
      r|= SCI_SCTE;
      regs[sci_scs1]->set(r);
      if (regs[sci_scc2]->get() & SCI_SCTIE)
	((cl_hc08*)uc)->request_irq(VEC_SCITX);
    }

  if (io->get_fin() && io->input_avail())
    io->proc_input(0);

  if (input_avail && !(regs[sci_scs1]->get() & SCI_SCRF))
    {
      u8_t c;
      if (get_input(&c))
	{
	  regs[sci_scdr]->set(c);
	  u32_t r= regs[sci_scs1]->get();
	  r|= SCI_SCRF;
	  regs[sci_scs1]->set(r);
	  if (regs[sci_scc2]->get() & SCI_SCRIE)
	    ((cl_hc08*)uc)->request_irq(VEC_SCIRX);
	}
    }
  return 0;
}

/* End of m68hc08.src/hw_sci.cc */
