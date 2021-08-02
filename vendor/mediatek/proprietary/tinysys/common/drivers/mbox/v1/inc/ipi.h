#ifndef __IPI_MBOX_H__
#define __IPI_MBOX_H__


#include "FreeRTOS.h"
#include "semphr.h"
#include "ipi_pin.h"
#include "mbox.h"

typedef struct ipi_action {
    void *data;
    unsigned int id;
    struct ipi_action *next;
} ipi_action_t;


#define IPI_REG_OK 0
#define IPI_REG_ALREADY -1
#define IPI_REG_ACTION_ERROR -2
#define IPI_REG_SEMAPHORE_FAIL -3

#define IPI_SERVICE_NOT_AVAILABLE -100

/* ipi_send() return code */
#define IPI_DONE             0
#define IPI_RETURN           1
#define IPI_BUSY            -1
#define IPI_TIMEOUT_AVL     -2
#define IPI_TIMEOUT_SEND    -3
#define IPI_MODULE_ID_ERROR -4
#define IPI_HW_ERROR        -5
#define IPI_NO_MEMORY       -6

extern int ipi_init(void);

/* defintion for opts argument */
#define IPI_OPT_DEFAUT          0
#define IPI_OPT_NO_WAIT_ACK     0x100

extern int ipi_recv_registration(int mid, ipi_action_t *act);
extern int ipi_recv_wait(int mid);
extern SemaphoreHandle_t ipi_get_wait(int mid);
extern int ipi_send_sync(int mid, int opts, void *buffer, int len, void *retbuf);
extern int ipi_send_sync_ex(int mid, int opts, void *buffer, int len,
                            void *retbuf, int retlen);
extern int ipi_send_async(int mid, int opts, void *buffer, int len);
extern int ipi_send_async_wait(int mid, int opts, void *retbuf);
extern int ipi_send_async_wait_ex(int mid, int opts, void *retbuf, int retlen);
extern int ipi_send_ack(int mid, unsigned int *data);
extern int ipi_send_ack_ex(int mid, void *data, int retlen);
#endif /* __IPI_MBOX_H__ */
