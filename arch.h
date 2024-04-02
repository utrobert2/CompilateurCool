#ifndef _ARCH_H_
#define _ARCH_H_

#define num_arch_registers 8
static int32_t max_regs = num_arch_registers;

void set_max_registers(int32_t n);
int32_t get_num_registers();
int32_t get_num_arch_registers();
int32_t get_first_reg();
int32_t get_r0();
int32_t get_stack_reg();
int32_t get_data_sec_start_addr();


#endif
