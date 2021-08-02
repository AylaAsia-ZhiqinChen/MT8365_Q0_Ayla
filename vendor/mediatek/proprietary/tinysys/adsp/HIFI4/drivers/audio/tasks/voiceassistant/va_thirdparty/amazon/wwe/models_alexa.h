// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

#ifndef PRL_MODEL_ALIGN
#ifdef _MSC_VER
#define PRL_MODEL_ALIGN _declspec(align(16))
#else
#define PRL_MODEL_ALIGN __attribute__((aligned(16)))
#endif
#endif // PRL_MODEL_ALIGN
#include <stddef.h>

extern char PRL_MODEL_ALIGN prlBinaryModelData[];
extern size_t prlBinaryModelLen;
