/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef KEY_H_
#define KEY_H_

#include <openssl/ossl_typ.h>

#define RSA_KEY_BITS		2048

/* Error codes */
enum {
	KEY_ERR_NONE,
	KEY_ERR_MALLOC,
	KEY_ERR_FILENAME,
	KEY_ERR_OPEN,
	KEY_ERR_LOAD
};

/* Supported key algorithms */
enum {
	KEY_ALG_RSA,
#ifndef OPENSSL_NO_EC
	KEY_ALG_ECDSA,
#endif /* OPENSSL_NO_EC */
	KEY_ALG_MAX_NUM
};

/*
 * This structure contains the relevant information to create the keys
 * required to sign the certificates.
 *
 * One instance of this structure must be created for each key, usually in an
 * array fashion. The filename is obtained at run time from the command line
 * parameters
 */
typedef struct key_s {
	int id;			/* Key id */
	const char *opt;	/* Command line option to specify a key */
	const char *help_msg;	/* Help message */
	const char *desc;	/* Key description (debug purposes) */
	char *fn;		/* Filename to load/store the key */
	EVP_PKEY *key;		/* Key container */
} key_t;

/* Exported API */
int key_init(void);
key_t *key_get_by_opt(const char *opt);
int key_new(key_t *key);
int key_create(key_t *key, int type);
int key_load(key_t *key, unsigned int *err_code);
int key_store(key_t *key);

/* Macro to register the keys used in the CoT */
#define REGISTER_KEYS(_keys) \
	key_t *keys = &_keys[0]; \
	const unsigned int num_keys = sizeof(_keys)/sizeof(_keys[0])

/* Exported variables */
extern key_t *keys;
extern const unsigned int num_keys;

#endif /* KEY_H_ */
