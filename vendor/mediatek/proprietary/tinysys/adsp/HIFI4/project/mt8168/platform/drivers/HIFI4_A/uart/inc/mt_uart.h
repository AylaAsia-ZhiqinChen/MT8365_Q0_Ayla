#ifndef MT_UART_H
#define MT_UART_H

#include <mt_reg_base.h>

//#define NO_USE_UART

#ifndef CFG_FPGA
#define UART_SRC_CLK        (26000000) //use in real chip
#else
#define UART_SRC_CLK        (10000000) //use in FPGA
#endif

#define UART_LOG_BAUD_RATE  (921600)
#define UART_LOG_PORT       DSP_UART0_BASE

#define UART_SET_BITS(REG, BS)       OUTREG32(REG, INREG32(REG) | (unsigned int)(BS))
#define UART_CLR_BITS(REG, BC)       OUTREG32(REG, INREG32(REG) & ~((unsigned int)(BC)))

#define UART_RBR(uart)                    ((uart)+0x0)  /* Read only */
#define UART_THR(uart)                    ((uart)+0x0)  /* Write only */
#define UART_IER(uart)                    ((uart)+0x4)
#define UART_IIR(uart)                    ((uart)+0x8)  /* Read only */
#define UART_FCR(uart)                    ((uart)+0x8)  /* Write only */
#define UART_LCR(uart)                    ((uart)+0xc)
#define UART_MCR(uart)                    ((uart)+0x10)
#define UART_LSR(uart)                    ((uart)+0x14)
#define UART_MSR(uart)                    ((uart)+0x18)
#define UART_SCR(uart)                    ((uart)+0x1c)
#define UART_DLL(uart)                    ((uart)+0x0)  /* Only when LCR.DLAB = 1 */
#define UART_DLH(uart)                    ((uart)+0x4)  /* Only when LCR.DLAB = 1 */
#define UART_EFR(uart)                    ((uart)+0x8)  /* Only when LCR = 0xbf */
#define UART_XON1(uart)                   ((uart)+0x10) /* Only when LCR = 0xbf */
#define UART_XON2(uart)                   ((uart)+0x14) /* Only when LCR = 0xbf */
#define UART_XOFF1(uart)                  ((uart)+0x18) /* Only when LCR = 0xbf */
#define UART_XOFF2(uart)                  ((uart)+0x1c) /* Only when LCR = 0xbf */
#define UART_AUTOBAUD_EN(uart)            ((uart)+0x20)
#define UART_HIGHSPEED(uart)              ((uart)+0x24)
#define UART_SAMPLE_COUNT(uart)           ((uart)+0x28)
#define UART_SAMPLE_POINT(uart)           ((uart)+0x2c)
#define UART_AUTOBAUD_REG(uart)           ((uart)+0x30)
#define UART_RATE_FIX_AD(uart)            ((uart)+0x34)
#define UART_AUTOBAUD_SAMPLE(uart)        ((uart)+0x38)
#define UART_GUARD(uart)                  ((uart)+0x3c)
#define UART_ESCAPE_DAT(uart)             ((uart)+0x40)
#define UART_ESCAPE_EN(uart)              ((uart)+0x44)
#define UART_SLEEP_EN(uart)               ((uart)+0x48)
#define UART_VFIFO_EN(uart)               ((uart)+0x4c) //ADSP_UART_RXDMA_EN_ADDR
#define UART_RXTRI_AD(uart)               ((uart)+0x50)
#define UART_FCR_RD(uart)                 ((uart)+0x5C)
#define UART_FEATURE_SEL(uart)            ((uart)+0x9C)

/* LSR */
#define UART_LSR_DR                 (1 << 0)
#define UART_LSR_OE                 (1 << 1)
#define UART_LSR_PE                 (1 << 2)
#define UART_LSR_FE                 (1 << 3)
#define UART_LSR_BI                 (1 << 4)
#define UART_LSR_THRE               (1 << 5)
#define UART_LSR_TEMT               (1 << 6)
#define UART_LSR_FIFOERR            (1 << 7)

/* LCR */
#define UART_LCR_BREAK              (1 << 6)
#define UART_LCR_DLAB               (1 << 7)

#define UART_WLS_5                  (0 << 0)
#define UART_WLS_6                  (1 << 0)
#define UART_WLS_7                  (2 << 0)
#define UART_WLS_8                  (3 << 0)
#define UART_WLS_MASK               (3 << 0)

#define UART_1_STOP                 (0 << 2)
#define UART_2_STOP                 (1 << 2)
#define UART_1_5_STOP               (1 << 2)    /* Only when WLS=5 */
#define UART_STOP_MASK              (1 << 2)

#define UART_NONE_PARITY            (0 << 3)
#define UART_ODD_PARITY             (0x1 << 3)
#define UART_EVEN_PARITY            (0x3 << 3)
#define UART_MARK_PARITY            (0x5 << 3)
#define UART_SPACE_PARITY           (0x7 << 3)
#define UART_PARITY_MASK            (0x7 << 3)

/* IER */
#define UART_IER_ERBFI              0x0001
#define UART_IER_ETBEI              0x0002
#define UART_IER_ELSI               0x0004
#define UART_IER_EDSSI              0x0008
#define UART_IER_XOFFI              0x0020
#define UART_IER_RTSI               0x0040
#define UART_IER_CTSI               0x0080
#define IER_HW_NORMALINTS           0x000d
#define IER_HW_ALLINTS              0x000f
#define IER_SW_NORMALINTS           0x002d
#define IER_SW_ALLINTS              0x002f

#define UART_IER_ALLOFF             0x0000
#define UART_IER_VFIFO              0x0001


/* FCR */
#define UART_FCR_FIFOE              (1 << 0)
#define UART_FCR_CLRR               (1 << 1)
#define UART_FCR_CLRT               (1 << 2)
#define UART_FCR_DMA1               (1 << 3)
#define UART_FCR_RXFIFO_1B_TRI      (0 << 6)
#define UART_FCR_RXFIFO_6B_TRI      (1 << 6)
#define UART_FCR_RXFIFO_12B_TRI     (2 << 6)
#define UART_FCR_RXFIFO_RX_TRI      (3 << 6)
#define UART_FCR_TXFIFO_1B_TRI      (0 << 4)
#define UART_FCR_TXFIFO_4B_TRI      (1 << 4)
#define UART_FCR_TXFIFO_8B_TRI      (2 << 4)
#define UART_FCR_TXFIFO_14B_TRI     (3 << 4)
#define UART_FCR_FIFO_INIT          (UART_FCR_FIFOE|UART_FCR_CLRR|UART_FCR_CLRT)
#define UART_FCR_NORMAL             (UART_FCR_FIFO_INIT|UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_12B_TRI)
#define UART_FCR_TRIGGER_LEVEL      (UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_6B_TRI)

/* IIR,RO */
#define   UART_IIR_INT_INVALID            0x0001
#define   UART_IIR_RLS                    0x0006  // Receiver Line Status
#define   UART_IIR_RDA                    0x0004  // Receive Data Available
#define   UART_IIR_CTI                    0x000C  // Character Timeout Indicator
#define   UART_IIR_THRE                   0x0002  // Transmit Holding Register Empty
#define   UART_IIR_MS                     0x0000  // Check Modem Status Register
#define   UART_IIR_SWFlowCtrl             0x0010  // Receive XOFF characters
#define   UART_IIR_HWFlowCtrl             0x0020  // CTS or RTS Rising Edge
#define   UART_IIR_FIFOS_ENABLED          0x00c0
#define   UART_IIR_NO_INTERRUPT_PENDING   0x0001
#define   UART_IIR_INT_MASK               0x003f

#define DEBUG_SERIAL_READ_NODATA   (0)

#define UART_CLK_CG_CTRL (ADSP_CK_CTRL_BASE + 0x10)
#define UART0_CG_ENABLE ((1<<5))
#define UART0_CLK ((1<<0))
#define UART0_RST (1<<3)


void uart_early_init(unsigned int g_uart, unsigned int speed,
               unsigned int uart_clk_changed);
void uart_register_irq(void);
void xSerialPortInitMinimal( unsigned portBASE_TYPE uxQueueLength );
signed portBASE_TYPE xSerialGetChar( signed char *pcRxedChar, TickType_t xBlockTime );

#endif /* MT_UART_H */
