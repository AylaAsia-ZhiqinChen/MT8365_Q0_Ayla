/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "FreeRTOS.h"
#include "queue.h"

#include <driver_api.h>
#include "main.h"
#include <mt_uart.h>
#include <interrupt.h>


void vUARTInterruptHandler(void);

#ifdef NO_USE_UART
void WriteDebugByte(unsigned int ch)
{
}
#else
void WriteDebugByte(unsigned int ch)
{
    unsigned int LSR;

    while (1) {
        LSR = INREG32(UART_LSR(UART_LOG_PORT));
        if (LSR & UART_LSR_THRE) {
            OUTREG32(UART_THR(UART_LOG_PORT), (unsigned int)ch);
            break;
        }
    }
}
#endif


#ifdef NO_USE_UART
int ReadDebugByte(void)
{
    return 0;
}
#else
int ReadDebugByte(void)
{
    unsigned int ch;
    unsigned int LSR;
    LSR = INREG32(UART_LSR(UART_LOG_PORT));
    if (LSR & UART_LSR_DR) {
        ch = (unsigned int)INREG32(UART_RBR(UART_LOG_PORT));
    } else {
        ch = DEBUG_SERIAL_READ_NODATA;
    }

    return (int)ch;
}
#endif

/*-----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
/*-----------------------------------------------------------*/

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE {
    WriteDebugByte(ch);
    return ch;
}

/**
  * @brief  Retargets the C library getc function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE {
    uint8_t RxByte;
    RxByte = ReadDebugByte();
    return RxByte;
}

#ifdef NO_USE_UART
void uart_early_init(unsigned int addr, unsigned int baudrate, unsigned int uart_clk)
{
}

void uart_register_irq(void)
{
}
#else
#define DIV_ROUND_CLOSEST(x, div)	(((x) + ((div) / 2)) / (div))
#define DIV_ROUND_UP(n, div)		(((n) + (div) - 1) / (div))
NORMAL_SECTION_FUNC void uart_early_init(unsigned int addr, unsigned int baudrate, unsigned int uart_clk)
{
    int highspeed, quot;
    int sample, sample_count, sample_point;
    int dll, dlh;
    unsigned int val;

    /* set baud rate */
    if (baudrate <= 115200) {
        highspeed = 0;
        quot = DIV_ROUND_CLOSEST(uart_clk, 16 * baudrate);
    } else if (baudrate <= 576000) {
        highspeed = 2;
        if ((baudrate == 500000) || (baudrate == 576000))
            baudrate = 460800;
        quot = DIV_ROUND_UP(uart_clk, 4 * baudrate);
    } else {
        highspeed = 3;
        quot = DIV_ROUND_UP(uart_clk, 256 * baudrate);
    }

    sample = DIV_ROUND_CLOSEST(uart_clk, quot * baudrate);
    sample_count = sample - 1;
    sample_point = (sample - 2) >> 1;
    dll = quot & 0xff;
    dlh = quot >> 8;

#if UART_NEW_FEATURE_SWITCH_OPTION
    DRV_WriteReg32(UART_FEATURE_SEL(addr), UART_FEATURE_SEL_NEW_MAP|UART_FEATURE_SEL_NO_DOWNLOAD);
#else
    DRV_WriteReg32(UART_FEATURE_SEL(addr), 0x80); //Disable download mode
#endif
    DRV_WriteReg32(UART_HIGHSPEED(addr), highspeed);

    DRV_WriteReg32(UART_LCR(addr), 0xBF);
    DRV_WriteReg32(UART_IIR(addr), 0x40);
    DRV_WriteReg32(UART_LCR(addr), UART_LCR_DLAB); // set LCR to DLAB to set DLL,DLH
    DRV_WriteReg32(UART_DLL(addr), dll);
    DRV_WriteReg32(UART_DLH(addr), dlh);
    DRV_WriteReg32(UART_LCR(addr), UART_WLS_8); //word length 8
    DRV_WriteReg32(UART_SAMPLE_COUNT(addr), sample_count);
    DRV_WriteReg32(UART_SAMPLE_POINT(addr), sample_point);

    val = UART_FCR_FIFO_INIT; /* Set FIFO */
#ifdef CFG_CLI_SUPPORT
    val |= UART_FCR_TRIGGER_LEVEL; /* Set trigger level */
#endif
    DRV_WriteReg32(UART_FCR(addr), val);

}

NORMAL_SECTION_FUNC void uart_register_irq(void)
{
#ifdef CFG_CLI_SUPPORT
    unsigned int base_addr = UART_LOG_PORT;

    DRV_WriteReg32(UART_IER(base_addr), UART_IER_ALLOFF);
    request_irq(LX_UART_IRQ_B, vUARTInterruptHandler, "UART0");
    DRV_WriteReg32(UART_IER(base_addr), UART_IER_ERBFI);
#endif
}

#ifdef CFG_CLI_SUPPORT
static QueueHandle_t xRxedChars;

NORMAL_SECTION_FUNC void xSerialPortInitMinimal( unsigned portBASE_TYPE uxQueueLength )
{
    /* Create the queues used to hold Rx characters */
    xRxedChars = xQueueCreate( uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char) );
}

signed portBASE_TYPE xSerialGetChar( signed char *pcRxedChar, TickType_t xBlockTime )
{
    /* Get the next character from the buffer. Return false if no characters
       are available, or arrive before xBlockTime expires. */
    if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ))
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

void vUARTInterruptHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
char cChar;
unsigned int iir;

    iir = INREG32( UART_IIR(UART_LOG_PORT) );
    if( iir & UART_IIR_RDA || iir & UART_IIR_CTI )
    {
        while( INREG32(UART_LSR(UART_LOG_PORT)) & (UART_LSR_DR) )
        {
            cChar = (char)INREG32(UART_RBR(UART_LOG_PORT));
            //WriteDebugByte(cChar);
            if ( !xRxedChars )
                continue;
            xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
        }
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
#endif /* CFG_CLI_SUPPORT */
#endif
