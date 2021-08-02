#ifndef __IPI_TYPES_H__
#define __IPI_TYPES_H__

/* pre-define slot and size here */
/* the array index is mapped to bit position of INT_IRQ or OUT_IRQ */
/* the slot is where the data begin */
/* the size is how many registers are needed */
typedef struct _pin_send {

    SemaphoreHandle_t mutex_send;  /* mutex */
    SemaphoreHandle_t sema_ack; /* binary semaphore */

    unsigned int mbox      : 3, /* mailbox number used by the pin*/
             slot      : 6, /* slot offset of the pin  */
             size      : 6, /* register number used to pass argument */
             shared    : 2, /* shared slot */
             retdata   : 1, /* return data or not */
             lock      : 3, /* Linux lock method: 0: mutex; 1: busy wait */
             polling   : 1, /* Linux ack polling method */
             unused    : 10;
    uint32_t     *prdata;
} pin_send_t;

typedef struct _shared_group {
    SemaphoreHandle_t sema_ack; /* binary semaphore */
    ipi_action_t *act;
} shared_group_t;

typedef struct _pin_recv {
    ipi_action_t *act;
    unsigned int mbox      : 3, /* mailbox number used by the pin*/
             slot      : 6, /* slot offset of the pin  */
             size      : 6, /* register number used to pass argument */
             shared    : 2, /* shared slot */
             retdata   : 2, /* return data or not */
             lock      : 2, /* Linux lock method: 0: mutex; 1: busy wait */
             share_grp : 5, /* shared group */
             unused    : 6;
} pin_recv_t;

typedef struct _mbox_info {
    unsigned int start : 8, /* start index of pin table*/
             end       : 8, /* end index of pin table*/
             used_slot : 8, /* used slots in the mailbox */
             mode      : 2, /* 0:disable, 1:for received, 2: for send */
             unused    : 6;
} mbox_info_t;

#define IPI_SLOT_SHARED 1
#define IPI_SLOT_NONE   0

#define IPI_DATA_RETURN 1
#define IPI_DATA_NONE   0


#endif /* __IPI_TYPES_H__ */
