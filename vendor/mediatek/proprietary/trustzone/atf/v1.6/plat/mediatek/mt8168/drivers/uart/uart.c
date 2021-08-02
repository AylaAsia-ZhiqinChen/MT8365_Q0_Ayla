#include <mmio.h>
#include <uart.h>

struct mtk_uart UART_SaveAddr[DRV_SUPPORT_UART_PORTS];

const unsigned int UART_BaseAddr[DRV_SUPPORT_UART_PORTS] = {
	0x11002000,
	0x11003000,
	0,
	0,
	0,
	0,
};

void mtk_uart_restore(void)
{
	unsigned long base;
	int uart_idx = UART_PORT0;
	struct mtk_uart *uart;

	/* Attention!! Must NOT print any debug log before UART restore */
	for (uart_idx = UART_PORT0; uart_idx < HW_SUPPORT_UART_PORTS; uart_idx++) {
		uart = &UART_SaveAddr[uart_idx];
		base = uart->base;

		mmio_write_32(UART_LCR(base), 0xbf);
		mmio_write_32(UART_EFR(base), uart->registers.efr);
		mmio_write_32(UART_LCR(base), uart->registers.lcr);
		mmio_write_32(UART_FCR(base), uart->registers.fcr);

		/* baudrate */
		mmio_write_32(UART_HIGHSPEED(base), uart->registers.highspeed);
		mmio_write_32(UART_FRACDIV_L(base), uart->registers.fracdiv_l);
		mmio_write_32(UART_FRACDIV_M(base), uart->registers.fracdiv_m);
		mmio_write_32(UART_LCR(base), uart->registers.lcr | UART_LCR_DLAB);
		mmio_write_32(UART_DLL(base), uart->registers.dll);
		mmio_write_32(UART_DLH(base), uart->registers.dlh);
		mmio_write_32(UART_LCR(base), uart->registers.lcr);
		mmio_write_32(UART_SAMPLE_COUNT(base), uart->registers.sample_count);
		mmio_write_32(UART_SAMPLE_POINT(base), uart->registers.sample_point);
		mmio_write_32(UART_GUARD(base), uart->registers.guard);

		/* flow control */
		mmio_write_32(UART_ESCAPE_EN(base), uart->registers.escape_en);
		mmio_write_32(UART_MCR(base), uart->registers.mcr);
		mmio_write_32(UART_IER(base), uart->registers.ier);
	}
}

void mtk_uart_save(void)
{
	int uart_idx = UART_PORT0;
	unsigned long base;
	struct mtk_uart *uart;

	for (uart_idx = UART_PORT0; uart_idx < HW_SUPPORT_UART_PORTS; uart_idx++) {
		UART_SaveAddr[uart_idx].base = UART_BaseAddr[uart_idx];
		base = UART_BaseAddr[uart_idx];
		uart = &UART_SaveAddr[uart_idx];
		uart->registers.lcr = mmio_read_32(UART_LCR(base));

		mmio_write_32(UART_LCR(base), 0xbf);
		uart->registers.efr = mmio_read_32(UART_EFR(base));
		mmio_write_32(UART_LCR(base), uart->registers.lcr);
		uart->registers.fcr = mmio_read_32(UART_FCR_RD(base));

		/* baudrate */
		uart->registers.highspeed = mmio_read_32(UART_HIGHSPEED(base));
		uart->registers.fracdiv_l = mmio_read_32(UART_FRACDIV_L(base));
		uart->registers.fracdiv_m = mmio_read_32(UART_FRACDIV_M(base));
		mmio_write_32(UART_LCR(base), uart->registers.lcr | UART_LCR_DLAB);
		uart->registers.dll = mmio_read_32(UART_DLL(base));
		uart->registers.dlh = mmio_read_32(UART_DLH(base));
		mmio_write_32(UART_LCR(base), uart->registers.lcr);
		uart->registers.sample_count = mmio_read_32(UART_SAMPLE_COUNT(base));
		uart->registers.sample_point = mmio_read_32(UART_SAMPLE_POINT(base));
		uart->registers.guard = mmio_read_32(UART_GUARD(base));

		/* flow control */
		uart->registers.escape_en = mmio_read_32(UART_ESCAPE_EN(base));
		uart->registers.mcr = mmio_read_32(UART_MCR(base));
		uart->registers.ier = mmio_read_32(UART_IER(base));
	}
}
