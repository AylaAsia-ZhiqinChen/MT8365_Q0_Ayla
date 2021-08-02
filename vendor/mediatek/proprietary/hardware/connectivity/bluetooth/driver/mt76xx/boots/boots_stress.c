//- vim: set ts=4 sts=4 sw=4 et: --------------------------------------------
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>

#include "boots.h"
#include "boots_stress.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_stress"

//---------------------------------------------------------------------------
static pthread_mutex_t boots_stress_thread_lock;
static int boots_stress_inited;
static pthread_t boots_stress_thread;
static uint32_t boots_stress_thread_status;
static uint32_t boots_stress_thread_should_stop;

static uint32_t boots_stress_timestamp_record_num;
static uint32_t* boots_stress_timestamp_record_send_cmd_start;
static uint32_t* boots_stress_timestamp_record_send_cmd_finish;
static uint32_t* boots_stress_timestamp_record_receive_event_finish;
static uint32_t boots_stress_timestamp_record_num_analysis;
static uint32_t* boots_stress_timestamp_record_send_cmd_start_analysis;
static uint32_t* boots_stress_timestamp_record_send_cmd_finish_analysis;
static uint32_t* boots_stress_timestamp_record_receive_event_finish_analysis;

static uint32_t boots_stress_timestamp_record_count_0us;
static uint32_t boots_stress_timestamp_record_count_1us;
static uint32_t boots_stress_timestamp_record_count_2us;
static uint32_t boots_stress_timestamp_record_count_3us;
static uint32_t boots_stress_timestamp_record_count_4us;
static uint32_t boots_stress_timestamp_record_count_5us;
static uint32_t boots_stress_timestamp_record_count_6us;
static uint32_t boots_stress_timestamp_record_count_7us;
static uint32_t boots_stress_timestamp_record_count_8us;
static uint32_t boots_stress_timestamp_record_count_9us;
static uint32_t boots_stress_timestamp_record_count_10us;
static uint32_t boots_stress_timestamp_record_count_20us;
static uint32_t boots_stress_timestamp_record_count_30us;
static uint32_t boots_stress_timestamp_record_count_40us;
static uint32_t boots_stress_timestamp_record_count_50us;
static uint32_t boots_stress_timestamp_record_count_60us;
static uint32_t boots_stress_timestamp_record_count_70us;
static uint32_t boots_stress_timestamp_record_count_80us;
static uint32_t boots_stress_timestamp_record_count_90us;
static uint32_t boots_stress_timestamp_record_count_100us;
static uint32_t boots_stress_timestamp_record_min;
static uint32_t boots_stress_timestamp_record_max;
static uint32_t boots_stress_timestamp_record_average;
static uint32_t boots_stress_timestamp_record_count_time;
static uint32_t boots_stress_timestamp_record_count_packet;

//---------------------------------------------------------------------------
static uint32_t boots_stress_get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    //BPRINT_I("%u", (uint32_t)(tv.tv_usec + tv.tv_sec * 1000000));
    return (tv.tv_usec + tv.tv_sec * 1000000);
}

static void boots_stress_mem_init(void)
{
    boots_stress_timestamp_record_num = 0;
    boots_stress_timestamp_record_num_analysis = 0;
    if (boots_stress_timestamp_record_send_cmd_start == NULL)
        boots_stress_timestamp_record_send_cmd_start =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    if (boots_stress_timestamp_record_send_cmd_finish == NULL)
        boots_stress_timestamp_record_send_cmd_finish =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    if (boots_stress_timestamp_record_receive_event_finish == NULL)
        boots_stress_timestamp_record_receive_event_finish =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    if (boots_stress_timestamp_record_send_cmd_start_analysis == NULL)
        boots_stress_timestamp_record_send_cmd_start_analysis =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    if (boots_stress_timestamp_record_send_cmd_finish_analysis == NULL)
        boots_stress_timestamp_record_send_cmd_finish_analysis =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    if (boots_stress_timestamp_record_receive_event_finish_analysis == NULL)
        boots_stress_timestamp_record_receive_event_finish_analysis =
                (uint32_t *)calloc(sizeof(uint32_t), BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);

    if (!boots_stress_timestamp_record_send_cmd_start || !boots_stress_timestamp_record_send_cmd_finish
            || !boots_stress_timestamp_record_receive_event_finish
            || !boots_stress_timestamp_record_send_cmd_start_analysis
            || !boots_stress_timestamp_record_send_cmd_finish_analysis
            || !boots_stress_timestamp_record_receive_event_finish_analysis) {
        BPRINT_E("%s allocate memory failed!", __func__);
        exit(0);
    }

    boots_stress_timestamp_record_count_0us = 0;
    boots_stress_timestamp_record_count_1us = 0;
    boots_stress_timestamp_record_count_2us = 0;
    boots_stress_timestamp_record_count_3us = 0;
    boots_stress_timestamp_record_count_4us = 0;
    boots_stress_timestamp_record_count_5us = 0;
    boots_stress_timestamp_record_count_6us = 0;
    boots_stress_timestamp_record_count_7us = 0;
    boots_stress_timestamp_record_count_8us = 0;
    boots_stress_timestamp_record_count_9us = 0;
    boots_stress_timestamp_record_count_10us = 0;
    boots_stress_timestamp_record_count_20us = 0;
    boots_stress_timestamp_record_count_30us = 0;
    boots_stress_timestamp_record_count_40us = 0;
    boots_stress_timestamp_record_count_50us = 0;
    boots_stress_timestamp_record_count_60us = 0;
    boots_stress_timestamp_record_count_70us = 0;
    boots_stress_timestamp_record_count_80us = 0;
    boots_stress_timestamp_record_count_90us = 0;
    boots_stress_timestamp_record_count_100us = 0;
    boots_stress_timestamp_record_min = 9999999;
    boots_stress_timestamp_record_max = 0;
    boots_stress_timestamp_record_average = 0;
    boots_stress_timestamp_record_count_time = 0;
    boots_stress_timestamp_record_count_packet = 0;
}

static void boots_stress_mem_free(void)
{
    if (boots_stress_timestamp_record_send_cmd_start == NULL)
        free(boots_stress_timestamp_record_send_cmd_start);
    if (boots_stress_timestamp_record_send_cmd_finish == NULL)
        free(boots_stress_timestamp_record_send_cmd_finish);
    if (boots_stress_timestamp_record_receive_event_finish == NULL)
        free(boots_stress_timestamp_record_receive_event_finish);
    if (boots_stress_timestamp_record_send_cmd_start_analysis == NULL)
        free(boots_stress_timestamp_record_send_cmd_start_analysis);
    if (boots_stress_timestamp_record_send_cmd_finish_analysis == NULL)
        free(boots_stress_timestamp_record_send_cmd_finish_analysis);
    if (boots_stress_timestamp_record_receive_event_finish_analysis == NULL)
        free(boots_stress_timestamp_record_receive_event_finish_analysis);
}

static void boots_stress_mem_copy(void)
{
    uint32_t buf_size = sizeof(uint32_t)*BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM;

    if (boots_stress_timestamp_record_send_cmd_start_analysis && boots_stress_timestamp_record_send_cmd_start)
        memcpy(boots_stress_timestamp_record_send_cmd_start_analysis, boots_stress_timestamp_record_send_cmd_start, buf_size);
    if (boots_stress_timestamp_record_send_cmd_finish_analysis && boots_stress_timestamp_record_send_cmd_finish)
        memcpy(boots_stress_timestamp_record_send_cmd_finish_analysis, boots_stress_timestamp_record_send_cmd_finish, buf_size);
    if (boots_stress_timestamp_record_receive_event_finish_analysis && boots_stress_timestamp_record_receive_event_finish)
        memcpy(boots_stress_timestamp_record_receive_event_finish_analysis, boots_stress_timestamp_record_receive_event_finish, buf_size);
    boots_stress_timestamp_record_num_analysis = boots_stress_timestamp_record_num;
}

static void boots_stress_mem_reset(void)
{
    uint32_t buf_size = sizeof(uint32_t) * BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM;

    boots_stress_timestamp_record_num = 0;
    if (boots_stress_timestamp_record_send_cmd_start)
        memset(boots_stress_timestamp_record_send_cmd_start, 0, buf_size);
    if (boots_stress_timestamp_record_send_cmd_finish)
        memset(boots_stress_timestamp_record_send_cmd_finish, 0, buf_size);
    if (boots_stress_timestamp_record_receive_event_finish)
        memset(boots_stress_timestamp_record_receive_event_finish, 0, buf_size);
}

static void boots_stress_mem_reset_analysis(void)
{
    uint32_t buf_size = sizeof(uint32_t) * BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM;

    boots_stress_timestamp_record_num_analysis = 0;
    if (boots_stress_timestamp_record_send_cmd_start_analysis)
        memset(boots_stress_timestamp_record_send_cmd_start_analysis, 0, buf_size);
    if (boots_stress_timestamp_record_send_cmd_finish_analysis)
        memset(boots_stress_timestamp_record_send_cmd_finish_analysis, 0, buf_size);
    if (boots_stress_timestamp_record_receive_event_finish_analysis)
        memset(boots_stress_timestamp_record_receive_event_finish_analysis, 0, buf_size);
}

static uint32_t boots_stress_mem_analysis(void)
{
    uint32_t i;
    uint32_t min_delta_time = 999999;
    uint32_t max_delta_time = 0;
    uint32_t avg_delta_time = 0;
    uint32_t iteration_delta_time = 0;
    static uint32_t time_counter = 0;

    if (boots_stress_timestamp_record_num_analysis == 0) {
        BPRINT_E("%s: Timestamp record num is 0 !", __func__);
        return 0;
    }
    if (boots_stress_timestamp_record_num_analysis >= BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM) {
        BPRINT_E("%s: Timestamp record num > BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM(%d)",
                __func__, BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
        return 0;
    }

    if (boots_stress_timestamp_record_num_analysis == 1) {
        max_delta_time = boots_stress_timestamp_record_receive_event_finish_analysis[0] - boots_stress_timestamp_record_send_cmd_start_analysis[0];
        BPRINT_I("[%d] Total count = %d (latency = %dus)",
                time_counter++,
                boots_stress_timestamp_record_num_analysis,
                max_delta_time);
        return 1;
    }

    // Skip first & latest entry since it's possible that the first or latest entry is not completed in this second.
    for (i = 1 ; i < boots_stress_timestamp_record_num_analysis - 1 ; i++) {
        if (boots_stress_timestamp_record_receive_event_finish_analysis[i] - boots_stress_timestamp_record_send_cmd_start_analysis[i] > max_delta_time)
            max_delta_time = boots_stress_timestamp_record_receive_event_finish_analysis[i] - boots_stress_timestamp_record_send_cmd_start_analysis[i];
        if (boots_stress_timestamp_record_receive_event_finish_analysis[i] - boots_stress_timestamp_record_send_cmd_start_analysis[i] < min_delta_time)
            min_delta_time = boots_stress_timestamp_record_receive_event_finish_analysis[i] - boots_stress_timestamp_record_send_cmd_start_analysis[i];

        iteration_delta_time = boots_stress_timestamp_record_receive_event_finish_analysis[i] - boots_stress_timestamp_record_send_cmd_start_analysis[i];
        avg_delta_time += iteration_delta_time;
        if (iteration_delta_time < 1000)
            boots_stress_timestamp_record_count_0us += 1;
        else if(iteration_delta_time < 2000)
            boots_stress_timestamp_record_count_1us += 1;
        else if(iteration_delta_time < 3000)
            boots_stress_timestamp_record_count_2us += 1;
        else if(iteration_delta_time < 4000)
            boots_stress_timestamp_record_count_3us += 1;
        else if(iteration_delta_time < 5000)
            boots_stress_timestamp_record_count_4us += 1;
        else if(iteration_delta_time < 6000)
            boots_stress_timestamp_record_count_5us += 1;
        else if(iteration_delta_time < 7000)
            boots_stress_timestamp_record_count_6us += 1;
        else if(iteration_delta_time < 8000)
            boots_stress_timestamp_record_count_7us += 1;
        else if(iteration_delta_time < 9000)
            boots_stress_timestamp_record_count_8us += 1;
        else if(iteration_delta_time < 10000)
            boots_stress_timestamp_record_count_9us += 1;
        else if(iteration_delta_time < 20000)
            boots_stress_timestamp_record_count_10us += 1;
        else if(iteration_delta_time < 30000)
            boots_stress_timestamp_record_count_20us += 1;
        else if(iteration_delta_time < 40000)
            boots_stress_timestamp_record_count_30us += 1;
        else if(iteration_delta_time < 50000)
            boots_stress_timestamp_record_count_40us += 1;
        else if(iteration_delta_time < 60000)
            boots_stress_timestamp_record_count_50us += 1;
        else if(iteration_delta_time < 70000)
            boots_stress_timestamp_record_count_60us += 1;
        else if(iteration_delta_time < 80000)
            boots_stress_timestamp_record_count_70us += 1;
        else if(iteration_delta_time < 90000)
            boots_stress_timestamp_record_count_80us += 1;
        else if(iteration_delta_time < 100000)
            boots_stress_timestamp_record_count_90us += 1;
        else
            boots_stress_timestamp_record_count_100us += 1;
    }
    avg_delta_time /= (boots_stress_timestamp_record_num_analysis - 2);
    BPRINT_I("[%d] Total count = %d (min:max:avg = %dus:%dus:%dus)",
                time_counter++,
                boots_stress_timestamp_record_num_analysis - 2,
                min_delta_time, max_delta_time, avg_delta_time);

    if ( max_delta_time > BOOTS_STRESS_MAX_ALLOWED_LATENCY)
    {
        BPRINT_E("!!!!!!!!!!! max_delta_time > %dms, Test Stopped !!!!!!", BOOTS_STRESS_MAX_ALLOWED_LATENCY/1000);
        boots_stress_thread_should_stop = 1;
    }

    boots_stress_timestamp_record_count_time++;
    boots_stress_timestamp_record_count_packet += (boots_stress_timestamp_record_num_analysis - 2);
    boots_stress_timestamp_record_average += avg_delta_time;
    if (min_delta_time < boots_stress_timestamp_record_min)
        boots_stress_timestamp_record_min = min_delta_time;
    if (max_delta_time > boots_stress_timestamp_record_max)
        boots_stress_timestamp_record_max = max_delta_time;

    return (boots_stress_timestamp_record_num_analysis - 2);
}

static void boots_stress_mem_record(uint8_t timestamp_type)
{
    if (boots_stress_timestamp_record_num < BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM) {
        pthread_mutex_lock(&boots_stress_thread_lock);
        switch (timestamp_type) {
            case BOOTS_STRESS_TIMESTAMP_SEND_CMD_START:
                boots_stress_timestamp_record_send_cmd_start[boots_stress_timestamp_record_num] = boots_stress_get_timestamp();
                break;
            case BOOTS_STRESS_TIMESTAMP_SEND_CMD_FINISH:
                boots_stress_timestamp_record_send_cmd_finish[boots_stress_timestamp_record_num] = boots_stress_get_timestamp();
                break;
            case BOOTS_STRESS_TIMESTAMP_RECEIVE_EVENT_FINISH:
                boots_stress_timestamp_record_receive_event_finish[boots_stress_timestamp_record_num] = boots_stress_get_timestamp();
                boots_stress_timestamp_record_num++;
                break;
            default:
                BPRINT_W("%s: unknow timestamp type! (%d)", __func__, timestamp_type);
                break;
        }
        pthread_mutex_unlock(&boots_stress_thread_lock);
    } else {
        BPRINT_W("%s: BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM(%d) should be increased!", __func__, BOOTS_STRESS_TIMESTAMP_RECORD_MAX_NUM);
    }
}

static void *boots_stress_thread_main(void *arg)
{
    uint32_t time_begin;
    uint32_t time_end;
    uint32_t test_iteration = 0;
    UNUSED(arg);

    boots_stress_thread_status = BOOTS_STRESS_THREAD_STATE_THREAD_RUNNING;
    usleep(1000000);
    while (!boots_stress_thread_should_stop) {
        time_begin = boots_stress_get_timestamp();

        pthread_mutex_lock(&boots_stress_thread_lock);
        boots_stress_mem_copy();
        boots_stress_mem_reset();
        pthread_mutex_unlock(&boots_stress_thread_lock);

        test_iteration = boots_stress_mem_analysis();
        if (test_iteration == 0 || test_iteration == 1) {
            boots_stress_thread_should_stop = 1;
            break;
        }

        boots_stress_mem_reset_analysis();
        time_end = boots_stress_get_timestamp();
        if ((1000000 - (time_end - time_begin)) > 0)
            usleep(1000000 - (time_end - time_begin));
    }
    boots_stress_thread_should_stop = 0;
    boots_stress_thread_status = BOOTS_STRESS_THREAD_STATE_THREAD_STOPPED;

    if (boots_stress_timestamp_record_count_packet > 1) {
        BPRINT_I("Bus Stress Test Result Summary : ");
        BPRINT_I("    Min Latency       : %d us", boots_stress_timestamp_record_min);
        BPRINT_I("    Max Latency       : %d us", boots_stress_timestamp_record_max);
        BPRINT_I("    Average Latency   : %d us", boots_stress_timestamp_record_average / boots_stress_timestamp_record_count_time);
        BPRINT_I("    Total Time        : %d seconds", boots_stress_timestamp_record_count_time);
        BPRINT_I("    Total Packets     : %d", boots_stress_timestamp_record_count_packet);
        if (boots_stress_timestamp_record_count_0us)
            BPRINT_I("        0ms ~ 1ms : %d", boots_stress_timestamp_record_count_0us);
        if (boots_stress_timestamp_record_count_1us)
            BPRINT_I("        1ms ~ 2ms : %d", boots_stress_timestamp_record_count_1us);
        if (boots_stress_timestamp_record_count_2us)
            BPRINT_I("        2ms ~ 3ms : %d", boots_stress_timestamp_record_count_2us);
        if (boots_stress_timestamp_record_count_3us)
            BPRINT_I("        3ms ~ 4ms : %d", boots_stress_timestamp_record_count_3us);
        if (boots_stress_timestamp_record_count_4us)
            BPRINT_I("        4ms ~ 5ms : %d", boots_stress_timestamp_record_count_4us);
        if (boots_stress_timestamp_record_count_5us)
            BPRINT_I("        5ms ~ 6ms : %d", boots_stress_timestamp_record_count_5us);
        if (boots_stress_timestamp_record_count_6us)
            BPRINT_I("        6ms ~ 7ms : %d", boots_stress_timestamp_record_count_6us);
        if (boots_stress_timestamp_record_count_7us)
            BPRINT_I("        7ms ~ 8ms : %d", boots_stress_timestamp_record_count_7us);
        if (boots_stress_timestamp_record_count_8us)
            BPRINT_I("        8ms ~ 9ms : %d", boots_stress_timestamp_record_count_8us);
        if (boots_stress_timestamp_record_count_9us)
            BPRINT_I("        9ms ~10ms : %d", boots_stress_timestamp_record_count_9us);
        if (boots_stress_timestamp_record_count_10us)
            BPRINT_I("       10ms ~20ms : %d", boots_stress_timestamp_record_count_10us);
        if (boots_stress_timestamp_record_count_20us)
            BPRINT_I("       20ms ~30ms : %d", boots_stress_timestamp_record_count_20us);
        if (boots_stress_timestamp_record_count_30us)
            BPRINT_I("       30ms ~40ms : %d", boots_stress_timestamp_record_count_30us);
        if (boots_stress_timestamp_record_count_40us)
            BPRINT_I("       40ms ~50ms : %d", boots_stress_timestamp_record_count_40us);
        if (boots_stress_timestamp_record_count_50us)
            BPRINT_I("       50ms ~60ms : %d", boots_stress_timestamp_record_count_50us);
        if (boots_stress_timestamp_record_count_60us)
            BPRINT_I("       60ms ~70ms : %d", boots_stress_timestamp_record_count_60us);
        if (boots_stress_timestamp_record_count_70us)
            BPRINT_I("       70ms ~80ms : %d", boots_stress_timestamp_record_count_70us);
        if (boots_stress_timestamp_record_count_80us)
            BPRINT_I("       80ms ~90ms : %d", boots_stress_timestamp_record_count_80us);
        if (boots_stress_timestamp_record_count_90us)
            BPRINT_I("       90ms~1000ms: %d", boots_stress_timestamp_record_count_90us);
        if (boots_stress_timestamp_record_count_100us)
            BPRINT_I("       > 100 ms   : %d", boots_stress_timestamp_record_count_100us);
    }

#if BOOTS_STRESS_MEASURE_IN_BOOTS
    BPRINT_I("    Measured Process  : boots");
    system("killall boots_srv > /dev/null");
#else
    BPRINT_I("    Measured Process  : boots_srv");
    system("killall boots > /dev/null");
#endif
    exit(0);
    return 0;
}

void boots_stress_record_timestamp(uint8_t timestamp_type)
{
    if (boots_stress_inited == 0)
        return;
    boots_stress_mem_record(timestamp_type);
}

void boots_stress_init(void)
{
    UNUSED(boots_btif);
    if (boots_stress_inited == 0) {
        pthread_mutex_init(&boots_stress_thread_lock, NULL);
        boots_stress_mem_init();
        pthread_create(&boots_stress_thread, NULL, boots_stress_thread_main, NULL);
        boots_stress_inited = 1;
    }
}

void boots_stress_deinit(void)
{
    if (boots_stress_inited == 1) {
        boots_stress_thread_should_stop = 1;
        pthread_join(boots_stress_thread, NULL);
        pthread_mutex_lock(&boots_stress_thread_lock);
        boots_stress_mem_free();
        pthread_mutex_unlock(&boots_stress_thread_lock);
        pthread_mutex_destroy(&boots_stress_thread_lock);
        boots_stress_inited = 0;
    }
}

