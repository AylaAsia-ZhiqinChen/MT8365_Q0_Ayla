/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#ifndef _PLF_TRACE_H_
#define _PLF_TRACE_H_
void ms_emi(const unsigned char cnt, unsigned int *value);
void ms_emi_tsct(const unsigned char cnt, unsigned int *value);
void ms_emi_mdct(const unsigned char cnt, unsigned int *value);

void ms_ttype(const unsigned char cnt, unsigned int *value);
void ms_bw_limiter(const unsigned char cnt, unsigned int *value);

void ms_dramc(const unsigned char cnt, unsigned int *value);
void ms_emi_ext(const unsigned char cnt, unsigned int *value);

#endif	/* _PLF_TRACE_H_ */
