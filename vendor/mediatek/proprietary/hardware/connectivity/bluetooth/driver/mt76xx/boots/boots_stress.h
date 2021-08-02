//- vim: set ts=4 sts=4 sw=4 et: --------------------------------------------
#ifndef __BOOTS_STRESS_H__
#define __BOOTS_STRESS_H__

// Print all CMD log in console
#define BOOTS_STRESS_SHOW_ALL_CMD 0
// Print all EVENT log in console
#define BOOTS_STRESS_SHOW_ALL_EVENT 0
// Measure the latency in 1:boots, 0:boots_srv
#define BOOTS_STRESS_MEASURE_IN_BOOTS 0
// The latency result includes :
//    1 : ACL_OUT + EVENT_IN(NOCP) + ACL_IN
//    0 : ACL_OUT + EVENT_IN(NOCP)
#define BOOTS_STRESS_MEASURE_LBT_TOTAL_LATENCY 0

// Maximum allowed latency (us)
#define BOOTS_STRESS_MAX_ALLOWED_LATENCY 50000
// Maximum records per second
#define BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM 2048
// Maximum stress test packet size (Max Local Name Size)
#define BOOTS_STRESS_STRESS_TEST_MAX_PKT_SIZE 248
// Maximum loopback test packet size (Max ACL Size)
#define BOOTS_STRESS_LOOPBACK_TEST_MAX_PKT_SIZE 1021

enum{
    BOOTS_STRESS_THREAD_STATE_UNKNOWN,
    BOOTS_STRESS_THREAD_STATE_THREAD_RUNNING,
    BOOTS_STRESS_THREAD_STATE_THREAD_STOPPED,
};

enum{
    BOOTS_STRESS_TIMESTAMP_SEND_CMD_START,
    BOOTS_STRESS_TIMESTAMP_SEND_CMD_FINISH,
    BOOTS_STRESS_TIMESTAMP_RECEIVE_EVENT_FINISH,
};

extern void boots_stress_record_timestamp(uint8_t timestamp_type);
extern void boots_stress_init(void);
extern void boots_stress_deinit(void);
#endif // __BOOTS_STRESS_H__