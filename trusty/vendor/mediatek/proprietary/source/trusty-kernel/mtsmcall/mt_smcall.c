/*
 * Copyright (c) 2015 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <arch/mmu.h>
#include <arch/ops.h>
#include <assert.h>
#include <debug.h>
#include <err.h>
#include <kernel/mp.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <lib/heap.h>
#include <lib/sm.h>
#include <lib/sm/smcall.h>

#include <lib/trusty/trusty_app.h>
#include <lib/trusty/uctx.h>
#include <lk/init.h>
#include <platform.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <trace.h>

#define LOCAL_TRACE 1

#define PRINT_TRUSTY_APP_UUID(u)					\
	dprintf(SPEW,							\
			"\tuuid: 0x%x 0x%x 0x%x 0x%x%x 0x%x%x%x%x%x%x\n",\
			(u)->time_low, (u)->time_mid,				\
			(u)->time_hi_and_version,				\
			(u)->clock_seq_and_node[0],				\
			(u)->clock_seq_and_node[1],				\
			(u)->clock_seq_and_node[2],				\
			(u)->clock_seq_and_node[3],				\
			(u)->clock_seq_and_node[4],				\
			(u)->clock_seq_and_node[5],				\
			(u)->clock_seq_and_node[6],				\
			(u)->clock_seq_and_node[7]);

extern uint als_slot_cnt;

#if WITH_MT_TRUSTY_DEBUGFS
static long smc_sc_add(smc32_args_t *args)
{
	uint cpu = arch_curr_cpu_num();
	long res;

	thread_sleep(500);

	res = args->params[0] + args->params[1];

	LTRACEF("(%d), %u + %u = %ld\n", cpu,
			args->params[0], args->params[1], res);
	return res;
}
#endif

static long smc_fc_threads(smc32_args_t *args)
{
	dprintf(INFO, "thread list:\n");
	dump_all_threads();
	return 0;
}

#if THREAD_STATS
static int smc_fc_threadstats(smc32_args_t *args)
{
	for (uint i = 0; i < SMP_MAX_CPUS; i++) {
		if (!(mp.active_cpus & (1 << i)))
			continue;

		printf("thread stats (cpu %d):\n", i);
		printf("\ttotal idle time: %lld\n", thread_stats[i].idle_time);
		printf("\ttotal busy time: %lld\n", current_time_hires() - thread_stats[i].idle_time);
		printf("\treschedules: %lu\n", thread_stats[i].reschedules);
#if WITH_SMP
		printf("\treschedule_ipis: %lu\n", thread_stats[i].reschedule_ipis);
#endif
		printf("\tcontext_switches: %lu\n", thread_stats[i].context_switches);
		printf("\tpreempts: %lu\n", thread_stats[i].preempts);
		printf("\tyields: %lu\n", thread_stats[i].yields);
		printf("\tinterrupts: %lu\n", thread_stats[i].interrupts);
		printf("\ttimer interrupts: %lu\n", thread_stats[i].timer_ints);
		printf("\ttimers: %lu\n", thread_stats[i].timers);
	}

	return 0;
}

static enum handler_return threadload(struct timer *t, lk_time_t now, void *arg)
{
	static struct thread_stats old_stats[SMP_MAX_CPUS];
	static lk_bigtime_t last_idle_time[SMP_MAX_CPUS];

	for (uint i = 0; i < SMP_MAX_CPUS; i++) {
		/* dont display time for inactiv cpus */
		if (!(mp.active_cpus & (1 << i)))
			continue;

		lk_bigtime_t idle_time = thread_stats[i].idle_time;

		/* if the cpu is currently idle, add the time since it went idle up until now to the idle counter */
		bool is_idle = !!(mp.idle_cpus & (1 << i));
		if (is_idle) {
			idle_time += current_time_hires() - thread_stats[i].last_idle_timestamp;
		}

		lk_bigtime_t delta_time = idle_time - last_idle_time[i];
		lk_bigtime_t busy_time = 1000000ULL - (delta_time > 1000000ULL ? 1000000ULL : delta_time);
		uint busypercent = (busy_time * 10000) / (1000000);

		printf("cpu %u LOAD: "
				"%u.%02u%%, "
				"cs %lu, "
				"pmpts %lu, "
#if WITH_SMP
				"rs_ipis %lu, "
#endif
				"ints %lu, "
				"tmr ints %lu, "
				"tmrs %lu\n",
				i,
				busypercent / 100, busypercent % 100,
				thread_stats[i].context_switches - old_stats[i].context_switches,
				thread_stats[i].preempts - old_stats[i].preempts,
#if WITH_SMP
				thread_stats[i].reschedule_ipis - old_stats[i].reschedule_ipis,
#endif
				thread_stats[i].interrupts - old_stats[i].interrupts,
				thread_stats[i].timer_ints - old_stats[i].timer_ints,
				thread_stats[i].timers - old_stats[i].timers);

		old_stats[i] = thread_stats[i];
		last_idle_time[i] = idle_time;
	}

	return INT_NO_RESCHEDULE;
}

static int smc_fc_threadload(smc32_args_t *args)
{
	static bool showthreadload = false;
	static timer_t tltimer;

	if (showthreadload == false) {
		// start the display
		timer_initialize(&tltimer);
		timer_set_periodic(&tltimer, 1000, &threadload, NULL);
		showthreadload = true;
	} else {
		timer_cancel(&tltimer);
		showthreadload = false;
	}

	return 0;
}
#endif // THREAD_STATS

static long smc_fc_heap_dump(smc32_args_t *args)
{
	// FIXME: heap_dump is static function in new Trusty
	//heap_dump();
	return 0;
}

static void app_dump(trusty_app_t *tapp, void *data)
{
	uctx_t *ctx = trusty_als_get(tapp, als_slot_cnt);
	trusty_app_props_t *props = &tapp->props;

	PRINT_TRUSTY_APP_UUID(&props->uuid);
	dprintf(INFO, "\t\tinuse: %08lx%08lx\n", mt_uctx_get_inuse(ctx, 1),
			mt_uctx_get_inuse(ctx, 0));
	dprintf(INFO, "\t\tmin_stack_size: 0x%x\n", props->min_stack_size);
	dprintf(INFO, "\t\tmin_heap_size: 0x%x\n", props->min_heap_size);
}

static long smc_fc_apps(smc32_args_t *args)
{
	trusty_app_forall(app_dump, NULL);
	return 0;
}

static long trusty_mt_sm_fastcall(smc32_args_t *args)
{
	long res;

	LTRACEF("Trusty OEM fastcall 0x%x args 0x%x 0x%x 0x%x\n",
			SMC_FUNCTION(args->smc_nr),
			args->params[0],
			args->params[1],
			args->params[2]);

	switch (args->smc_nr) {

		case MT_SMC_FC_THREADS:
			res = smc_fc_threads(args);
			break;

#if THREAD_STATS
		case MT_SMC_FC_THREADSTATS:
			res = smc_fc_threadstats(args);
			break;

		case MT_SMC_FC_THREADLOAD:
			res = smc_fc_threadload(args);
			break;
#endif // THREAD_STATS

		case MT_SMC_FC_HEAP_DUMP:
			res = smc_fc_heap_dump(args);
			break;

		case MT_SMC_FC_APPS:
			res = smc_fc_apps(args);
			break;

		default:
			LTRACEF("unknown func 0x%x\n", SMC_FUNCTION(args->smc_nr));
			res = ERR_NOT_SUPPORTED;
			break;
	}
	return res;

}

static long trusty_mt_sm_stdcall(smc32_args_t *args)
{
	long res;

	LTRACEF("Trusty OEM stdcall 0x%x args 0x%x 0x%x 0x%x\n",
			SMC_FUNCTION(args->smc_nr),
			args->params[0],
			args->params[1],
			args->params[2]);

	switch (args->smc_nr) {

#if WITH_MT_TRUSTY_DEBUGFS
		case MT_SMC_SC_ADD:
			res = smc_sc_add(args);
			break;
#endif

		default:
			LTRACEF("unknown func 0x%x\n", SMC_FUNCTION(args->smc_nr));
			res = ERR_NOT_SUPPORTED;
			break;
	}

	return res;
}

static smc32_entity_t trusty_mt_sm_entity = {
	.fastcall_handler = trusty_mt_sm_fastcall,
	.stdcall_handler = trusty_mt_sm_stdcall,
};

static void trusty_mt_sm_init(uint level)
{
	int err;

	dprintf(INFO, "Initializing MTK SMC handler\n");

	err = sm_register_entity(SMC_ENTITY_MT_TRUSTED_OS, &trusty_mt_sm_entity);
	if (err) {
		TRACEF("WARNING: Cannot register SMC entity! (%d)\n", err);
	}
}
LK_INIT_HOOK(trusty_mt_smcall, trusty_mt_sm_init, LK_INIT_LEVEL_APPS);

