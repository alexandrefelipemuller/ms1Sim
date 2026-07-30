/*
 * Simulator of microcontrollers (m68hc08.src/hw_adc.cc)
 *
 * Minimal ADC stub for the 68HC908GP32 register map.
 */

#include "m68hc08cl.h"
#include "hw_adc.h"

#define ADC_COCO 0x80
#define ADC_AIEN 0x40

#define VEC_ADC ((t_addr)0xFFDE)


cl_adc::cl_adc(class cl_uc *auc, int aid, t_addr abase):
  cl_hw(auc, HW_DUMMY, aid, "adc")
{
  base= abase;
  conversion_pending= false;
}

int
cl_adc::init(void)
{
  regs[adc_adscr]= register_cell(uc->rom, base+adc_adscr);
  regs[adc_adr]=   register_cell(uc->rom, base+adc_adr);
  regs[adc_adclk]= register_cell(uc->rom, base+adc_adclk);
  regs[adc_adscr]->set(ADC_COCO);
  regs[adc_adr]->set(0x80);
  return(0);
}

t_mem
cl_adc::read(class cl_memory_cell *cell)
{
  return cell->get();
}

void
cl_adc::write(class cl_memory_cell *cell, t_mem *val)
{
  cell->set(*val);
  if (cell == regs[adc_adscr])
    conversion_pending= true;
}

int
cl_adc::tick(int cycles)
{
  if (conversion_pending)
    {
      u32_t r= regs[adc_adscr]->get();
      r|= ADC_COCO;
      regs[adc_adscr]->set(r);
      regs[adc_adr]->set(0x80);
      conversion_pending= false;
      if (r & ADC_AIEN)
	((cl_hc08*)uc)->request_irq(VEC_ADC);
    }
  return 0;
}

/* End of m68hc08.src/hw_adc.cc */
