#include "dramc_api.h"

#if (FOR_DV_SIMULATION_USED == 1)
unsigned int register_read_c(unsigned int *addr)
{
	return 0;
}

void register_write_c(unsigned int *addr, unsigned int data)
{
}

void delay_us(unsigned int us)
{}

void show_current_time(void)
{
}

#define NULL ((void *)0)

void main(void)
{
	dpi_simulation_dramc(NULL);
}
#endif
