/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/decoder.h>
#include "crypt.h"
#include "keys.h"
#include "runconfig.h"

extern "C" {
int HyvCryptRsaEnc(const unsigned char* ibuf, size_t ilen, unsigned char* obuf, size_t olen, unsigned int key_id) {
	if (ibuf == NULL || ilen == 0) {
		fprintf(stderr, "Input buffer not provided\n");
		return -1;
	}
	if (obuf == NULL || olen == 0) {
		fprintf(stderr, "Output buffer not provided\n");
		return -1;
	}
	char path_buf[4096];
	path_buf[4095] = '\0';
	const config_t* config = globalConfig->getConfig();
	FILE* fp = NULL;
	EVP_PKEY* pkey = NULL;
	OSSL_DECODER_CTX* fctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, NULL, NULL, "RSA", 0, NULL, NULL);
	const unsigned char* key;
	size_t keylen;
	if (fctx == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	// 0: Signing 1: Auth 2+: Dispatch
	switch (key_id) {
	case 2 ... 5:
		key = DispatchKeys[key_id];
		/* TODO Don't hardcode the key buffer length */
		keylen = 1192;
		if (key == NULL) {
			fprintf(stderr, "Invalid key ID\n");
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_data(fctx, &key, &keylen)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		break;
	case 0:
		snprintf(path_buf, 4095, "%s/keys/signingKey.pem", config->dataPath);
		fp = fopen(path_buf, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s (%s)\n", path_buf, strerror(errno));
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_fp(fctx, fp)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		fclose(fp);
		fp = NULL;
		break;
	case 1:
		snprintf(path_buf, 4095, "%s/keys/authKey.pem", config->dataPath);
		fp = fopen(path_buf, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s (%s)\n", path_buf, strerror(errno));
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_fp(fctx, fp)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		fclose(fp);
		fp = NULL;
		break;
	default:
		fprintf(stderr, "Invalid key ID\n");
		OSSL_DECODER_CTX_free(fctx);
		return -1;
	}
	OSSL_DECODER_CTX_free(fctx);
	if (pkey == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate the encryption context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	if (EVP_PKEY_encrypt_init(ctx) <= 0) {
		fprintf(stderr, "Unable to init the encryption context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
		fprintf(stderr, "Unable to set the padding mode; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	size_t obs;
	/* Test encrypt a single byte of input data to determine output block size for a given key */
	if (EVP_PKEY_encrypt(ctx, NULL, &obs, ibuf, 1) <= 0) {
		fprintf(stderr, "Unable to get step size for encrypted data; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	size_t ibs = obs - 11; /* 3 byte padding format + 64 bit random data */
	size_t i = 0;
	while ((ssize_t) ilen > 0 && (ssize_t) olen > 0) {
		if (EVP_PKEY_encrypt(ctx, obuf, &obs, ibuf, ilen < ibs ? ilen : ibs) <= 0) {
			fprintf(stderr, "Unable to encrypt the data; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			EVP_PKEY_CTX_free(ctx);
			return -1;
		}
		i++;
		ibuf += ibs;
		obuf += obs;
		ilen -= ibs;
		olen -= obs;
	}
	EVP_PKEY_CTX_free(ctx);
	fprintf(stderr, "Successfully encrypted %ld blocks of data\n", i);
	return i * obs;
}

int HyvCryptRsaDec(const unsigned char* ibuf, size_t ilen, unsigned char* obuf, size_t olen, unsigned int key_id) {
	if (ibuf == NULL || ilen == 0) {
		fprintf(stderr, "Input buffer not provided\n");
		return -1;
	}
	if (obuf == NULL || olen == 0) {
		fprintf(stderr, "Output buffer not provided\n");
		return -1;
	}
	char path_buf[4096];
	path_buf[4095] = '\0';
	const config_t* config = globalConfig->getConfig();
	FILE* fp = NULL;
	EVP_PKEY* pkey = NULL;
	OSSL_DECODER_CTX* fctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, NULL, NULL, "RSA", 0, NULL, NULL);
	const unsigned char* key;
	size_t keylen;
	if (fctx == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	// 0: Signing 1: Auth 2+: Dispatch
	switch (key_id) {
	case 2 ... 5:
		key = DispatchKeys[key_id];
		/* TODO Don't hardcode the key buffer length */
		keylen = 1192;
		if (key == NULL) {
			fprintf(stderr, "Invalid key ID\n");
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_data(fctx, &key, &keylen)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		break;
	case 0:
		snprintf(path_buf, 4095, "%s/keys/signingKey.pem", config->dataPath);
		fp = fopen(path_buf, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s (%s)\n", path_buf, strerror(errno));
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_fp(fctx, fp)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		fclose(fp);
		fp = NULL;
		break;
	case 1:
		snprintf(path_buf, 4095, "%s/keys/authKey.pem", config->dataPath);
		fp = fopen(path_buf, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s (%s)\n", path_buf, strerror(errno));
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		if (!OSSL_DECODER_from_fp(fctx, fp)) {
			fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			OSSL_DECODER_CTX_free(fctx);
			return -1;
		}
		fclose(fp);
		fp = NULL;
		break;
	default:
		fprintf(stderr, "Invalid key ID\n");
		OSSL_DECODER_CTX_free(fctx);
		return -1;
	}
	OSSL_DECODER_CTX_free(fctx);
	if (pkey == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate the decryption context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		fprintf(stderr, "Unable to init the decryption context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
		fprintf(stderr, "Unable to set the padding mode; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	size_t obs;
	if (EVP_PKEY_decrypt(ctx, NULL, &obs, ibuf, 1) <= 0) {
		fprintf(stderr, "Unable to get step size for decrypted data; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}
	size_t ibs = obs + 11;
	size_t i = 0;
	while ((ssize_t) ilen > 0 && (ssize_t) olen > 0) {
		if (EVP_PKEY_decrypt(ctx, obuf, &obs, ibuf, ilen < ibs ? ilen : ibs) <= 0) {
			fprintf(stderr, "Unable to decrypt the data; libcrypto errors follow\n");
			ERR_print_errors_fp(stderr);
			EVP_PKEY_CTX_free(ctx);
			return -1;
		}
		i++;
		ibuf += ibs;
		obuf += obs;
		ilen -= ibs;
		olen -= obs;
	}
	EVP_PKEY_CTX_free(ctx);
	fprintf(stderr, "Successfully decrypted %ld blocks of data\n", i);
	return i * obs;
}

int HyvCryptRsaSign(const unsigned char* ibuf, size_t ilen, unsigned char* obuf, size_t olen) {
	if (ibuf == NULL || ilen == 0) {
		fprintf(stderr, "Input buffer not provided\n");
		return -1;
	}
	if (obuf == NULL || olen == 0) {
		fprintf(stderr, "Output buffer not provided\n");
		return -1;
	}
	char path_buf[4096];
	path_buf[4095] = '\0';
	const config_t* config = globalConfig->getConfig();
	EVP_PKEY* pkey = NULL;
	OSSL_DECODER_CTX* fctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, NULL, NULL, "RSA", 0, NULL, NULL);
	if (fctx == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	snprintf(path_buf, 4095, "%s/keys/signingKey.pem", config->dataPath);
	FILE* fp = fopen(path_buf, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Unable to open %s (%s)\n", path_buf, strerror(errno));
		OSSL_DECODER_CTX_free(fctx);
		return -1;
	}
	if (!OSSL_DECODER_from_fp(fctx, fp)) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		OSSL_DECODER_CTX_free(fctx);
		return -1;
	}
	fclose(fp);
	fp = NULL;
	OSSL_DECODER_CTX_free(fctx);
	if (pkey == NULL) {
		fprintf(stderr, "Unable to init the key; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	EVP_PKEY_CTX* pctx = NULL;
	EVP_MD_CTX* mctx = EVP_MD_CTX_new();
	if (mctx == NULL) {
		fprintf(stderr, "Unable to allocate the digest context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}
	EVP_MD* md = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (md == NULL) {
		fprintf(stderr, "This libcrypto doesn't support sha256 for some reason; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_MD_CTX_free(mctx);
		return -1;
	}
	if (EVP_DigestSignInit(mctx, &pctx, md, NULL, pkey) <= 0) {
		fprintf(stderr, "Unable to init the digest context; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_MD_free(md);
		EVP_MD_CTX_free(mctx);
		return -1;
	}
	if (EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PADDING) <= 0) {
		fprintf(stderr, "Unable to set the padding mode; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_MD_free(md);
		EVP_MD_CTX_free(mctx);
		return -1;
	}
	if (EVP_DigestSign(mctx, obuf, &olen, ibuf, ilen) <= 0) {
		fprintf(stderr, "Unable to sign the input data; libcrypto errors follow\n");
		ERR_print_errors_fp(stderr);
		EVP_MD_free(md);
		EVP_MD_CTX_free(mctx);
		return -1;
	}
	fprintf(stderr, "Signed %ld bytes of data\n", ilen);
	EVP_MD_free(md);
	EVP_MD_CTX_free(mctx);
	return olen;
}

int HyvCryptXor(unsigned char* ibuf, size_t ilen, const unsigned char* kbuf, size_t klen) {
	unsigned int i;
	for (i = 0; i < ilen; i++) {
		ibuf[i] ^= kbuf[i % klen];
	}
	return ilen;
}
}
