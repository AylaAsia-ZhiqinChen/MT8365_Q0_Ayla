// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include "met_drv.h"
#include "interface.h"
#include "trace.h"
extern char *ms_formatH_EOL(char *__restrict__ buf, unsigned char cnt, unsigned int *__restrict__ value);
noinline void ms_emi(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}

noinline void ms_emi_ext(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}

noinline void ms_emi_tsct(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}

noinline void ms_emi_mdct(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}


noinline void ms_ttype(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}
noinline void ms_dramc(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}
noinline void ms_bw_limiter(const unsigned char cnt, unsigned int *value)
{
	char *SOB, *EOB;

	MET_TRACE_GETBUF(&SOB, &EOB);
	// coverity[var_deref_op : FALSE]
	EOB = ms_formatH_EOL(EOB, cnt, value);
	MET_TRACE_PUTBUF(SOB, EOB);
}

