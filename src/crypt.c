/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include "crypt.h"
#include "keys.h"

int HyvCryptRsaEnc(const unsigned char* ibuf, size_t ilen, unsigned char* obuf, size_t olen, unsigned int key_id) {
	if (ibuf == NULL || ilen == 0) {
		fprintf(stderr, "Input buffer not provided\n");
		return -1;
	}
	if (obuf == NULL || olen == 0) {
		fprintf(stderr, "Output buffer not provided\n");
		return -1;
	}
	if (key_id > 5) {
		fprintf(stderr, "Invalid key ID\n");
		return -1;
	}
	const unsigned char* const key = DispatchKeys[key_id];
	if (key == NULL) {
		fprintf(stderr, "Invalid key ID\n");
		return -1;
	}
	/* TODO Don't hardcode the key buffer length */
	EVP_PKEY* pkey = d2i_PrivateKey(EVP_PKEY_RSA, NULL, (const unsigned char**) &key, 1192);
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
	fprintf(stderr, "Successfully encrypted %d blocks of data\n", i);
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
	const unsigned char* key = signPrivKey;
	/* TODO Don't hardcode the key buffer length */
	EVP_PKEY* pkey = d2i_PrivateKey(EVP_PKEY_RSA, NULL, &key, 1193);
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
	fprintf(stderr, "Signed %d bytes of data\n", ilen);
	EVP_MD_free(md);
	EVP_MD_CTX_free(mctx);
	return olen;
}
