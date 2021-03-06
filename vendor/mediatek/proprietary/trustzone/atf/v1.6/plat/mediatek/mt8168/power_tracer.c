/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <arch.h>
#include <debug.h>
#include <power_tracer.h>

#define trace_log(...)  INFO("psci: " __VA_ARGS__)

void trace_power_flow(unsigned long mpidr, unsigned char mode)
{
	switch (mode) {
	case CPU_UP:
		trace_log("core %lld:%lld ON\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS,
			  (mpidr & MPIDR_CPU_MASK));
		break;
	case CPU_DOWN:
		trace_log("core %lld:%lld OFF\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS,
			  (mpidr & MPIDR_CPU_MASK));
		break;
	case CPU_SUSPEND:
		trace_log("core %lld:%lld SUSPEND\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS,
			  (mpidr & MPIDR_CPU_MASK));
		break;
	case CLUSTER_UP:
		trace_log("cluster %lld ON\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS);
		break;
	case CLUSTER_DOWN:
		trace_log("cluster %lld OFF\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS);
		break;
	case CLUSTER_SUSPEND:
		trace_log("cluster %lld SUSPEND\n",
			  (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFFINITY_BITS);
		break;
	default:
		trace_log("unknown power mode\n");
		break;
	}
}
