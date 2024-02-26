/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <sys/random.h>
#include <sys/types.h>
#include <assert.h>
#include <string>
#include "ec2b.h"
#include "mt19937-64.h"

extern "C" {
	static void scrambleKey(unsigned char[16]);
}

Ec2b::Ec2b() {}
Ec2b::~Ec2b() {}

Ec2b::Ec2b(int r) {
	if (r) {
		char _key[16];
		getrandom(_key, 16, 0);
		key.assign(_key, 16);
		char _data[2048];
		getrandom(_data, 2048, 0);
		data.assign(_data, 2048);
		deriveXor();
	}
}

Ec2b::Ec2b(const char* path) {
	if (path != NULL) {
		FILE* f = fopen(path, "rb");
		if (f != NULL) {
			char buf[0x81c];
			memset(buf, 0, 0x81c);
			fread(buf, 1, 0x81c, f);
			fclose(f);
			const ec2b_t* ec2b = (const ec2b_t*) buf;
			assert(ec2b->magic == be32toh(0x45633262));
			assert(ec2b->keyLen == 16);
			assert(ec2b->dataLen == 2048);
			key.assign((const char*) ec2b->key, ec2b->keyLen);
			data.assign((const char*) ec2b->data, ec2b->dataLen);
			deriveXor();
		}
	}
}

Ec2b::Ec2b(FILE* f) {
	if (f != NULL) {
		size_t cachedSeek = ftell(f);
		fseek(f, 0, SEEK_SET);
		char buf[0x81c];
		memset(buf, 0, 0x81c);
		fread(buf, 1, 0x81c, f);
		fseek(f, cachedSeek, SEEK_SET);
		const ec2b_t* ec2b = (const ec2b_t*) buf;
		assert(ec2b->magic == be32toh(0x45633262));
		assert(ec2b->keyLen == 16);
		assert(ec2b->dataLen == 2048);
		key.assign((const char*) ec2b->key, ec2b->keyLen);
		data.assign((const char*) ec2b->data, ec2b->dataLen);
		deriveXor();
	}
}

Ec2b::Ec2b(const std::string& buf) {
	assert(buf.size() >= 0x81c);
	const ec2b_t* ec2b = (const ec2b_t*) buf.c_str();
	assert(ec2b->magic == be32toh(0x45633262));
	assert(ec2b->keyLen == 16);
	assert(ec2b->dataLen == 2048);
	key.assign((const char*) ec2b->key, ec2b->keyLen);
	data.assign((const char*) ec2b->data, ec2b->dataLen);
	deriveXor();
}

Ec2b::Ec2b(const char* buf, size_t sz) {
	if (buf != NULL) {
		assert(sz >= 0x81c);
		const ec2b_t* ec2b = (const ec2b_t*) buf;
		assert(ec2b->magic == be32toh(0x45633262));
		assert(ec2b->keyLen == 16);
		assert(ec2b->dataLen == 2048);
		key.assign((const char*) ec2b->key, ec2b->keyLen);
		data.assign((const char*) ec2b->data, ec2b->dataLen);
		deriveXor();
	}
}

Ec2b::Ec2b(ec2b_t ec2b) {
	assert(ec2b.magic == be32toh(0x45633262));
	assert(ec2b.keyLen == 16);
	assert(ec2b.dataLen == 2048);
	key.assign((const char*) ec2b.key, ec2b.keyLen);
	data.assign((const char*) ec2b.data, ec2b.dataLen);
	deriveXor();
}

Ec2b::Ec2b(const ec2b_t* ec2b) {
	if (ec2b != NULL) {
		assert(ec2b->magic == be32toh(0x45633262));
		assert(ec2b->keyLen == 16);
		assert(ec2b->dataLen == 2048);
		key.assign((const char*) ec2b->key, ec2b->keyLen);
		data.assign((const char*) ec2b->data, ec2b->dataLen);
		deriveXor();
	}
}

Ec2b::Ec2b(const std::string& _key, const std::string& _data) {
	assert(_key.size() >= 16);
	assert(_data.size() >= 2048);
	key = _key;
	data = _data;
}

Ec2b::Ec2b(const char* _key, size_t keySz, const char* _data, size_t dataSz) {
	assert(keySz >= 16);
	assert(dataSz >= 2048);
	key.assign(_key, keySz);
	data.assign(_data, dataSz);
}

const std::string& Ec2b::getKey() const {
	return key;
}

void Ec2b::setKey(const std::string& k) {
	assert(k.size() >= 16);
	key = k;
	deriveXor();
}

const std::string& Ec2b::getData() const {
	return data;
}

void Ec2b::setData(const std::string& d) {
	assert(d.size() >= 2048);
	data = d;
	deriveXor();
}

const std::string& Ec2b::getXorpad() const {
	return xorpad;
}

unsigned long long Ec2b::getSeed() const {
	return seed;
}

Ec2b::operator ec2b_t() const {
	ec2b_t ret;
	ret.magic = htobe32(0x45633262);
	ret.keyLen = 16;
	ret.dataLen = 2048;
	memcpy(ret.key, key.c_str(), 16);
	memcpy(ret.data, data.c_str(), 2048);
	return ret;
}

Ec2b::operator std::string() const {
	ec2b_t ret = *this;
	std::string retS((const char*) &ret, sizeof(ec2b_t));
	return retS;
}

void Ec2b::deriveXor() {
	unsigned char _xorpad[4096];
	unsigned char k[16];
	memcpy(k, key.c_str(), 16);
	scrambleKey(k);
	unsigned long long _seed = ~0xceac3b5a867837ac;
	unsigned long long* _k = (unsigned long long*) k;
	_seed ^= _k[0];
	_seed ^= _k[1];
	const unsigned long long* d = (const unsigned long long*) data.c_str();
	unsigned int i;
	for (i = 0; i < 256; i++) {
		_seed ^= d[i];
	}
	seed = _seed;
	genXorpadFromSeed(seed, _xorpad, 4096);
	xorpad.assign((const char*) _xorpad, 4096);
}

extern "C" {
	static const unsigned char aesRoundKeys[11][16] = {
		{0x3c, 0x5e, 0xad, 0x0f, 0xd5, 0x09, 0x27, 0x3f, 0xb8, 0x70, 0x00, 0x9a, 0xcd, 0x30, 0x1b, 0xeb},
		{0xb7, 0x04, 0x71, 0xd9, 0x39, 0x80, 0x21, 0x29, 0xb5, 0xcc, 0x7a, 0xb2, 0xae, 0xb6, 0x75, 0x14},
		{0x63, 0x2a, 0x82, 0x34, 0x70, 0xa5, 0x40, 0xeb, 0xf9, 0x4e, 0x95, 0x1c, 0x0a, 0xa4, 0xd0, 0xf6},
		{0x56, 0x1e, 0x0e, 0xe3, 0x7b, 0xbf, 0x0d, 0xc5, 0xd3, 0x04, 0xf3, 0x43, 0xda, 0x76, 0x37, 0xdd},
		{0xad, 0xe9, 0xf6, 0x97, 0x54, 0xd2, 0x56, 0xa2, 0x00, 0xbe, 0x96, 0xd0, 0x61, 0x4f, 0x8a, 0xbe},
		{0x5c, 0x32, 0x74, 0xc8, 0xfd, 0x7f, 0x2c, 0xfc, 0x5d, 0x4e, 0xd0, 0x6b, 0x2a, 0x2b, 0xf8, 0xde},
		{0x12, 0x5b, 0xa2, 0x58, 0x8c, 0x4e, 0x02, 0xe5, 0x3c, 0xa6, 0xdb, 0x02, 0xbf, 0xaa, 0xe5, 0x12},
		{0xe0, 0xef, 0x09, 0x36, 0xf6, 0xa0, 0xe5, 0x60, 0xe1, 0x62, 0xe4, 0x54, 0x02, 0xa7, 0xd1, 0x71},
		{0xc0, 0xf6, 0xe0, 0xff, 0xdd, 0x01, 0xba, 0xd5, 0x26, 0x94, 0x2d, 0x85, 0xa3, 0x7d, 0xdf, 0x0f},
		{0x94, 0x2f, 0xd6, 0x39, 0xe6, 0xec, 0xca, 0x86, 0x73, 0xd5, 0x66, 0x6a, 0x98, 0x92, 0x86, 0xce},
		{0x20, 0xb4, 0xf0, 0x4c, 0xaa, 0xdd, 0x5a, 0xd5, 0x78, 0x2c, 0x81, 0xbe, 0xae, 0x3a, 0x31, 0x14}
	};

	static const unsigned char sbox[256] = {
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};

	static const unsigned char mul9tbl[256] = {
	    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
	    0x90, 0x99, 0x82, 0x8b, 0xb4, 0xbd, 0xa6, 0xaf, 0xd8, 0xd1, 0xca, 0xc3, 0xfc, 0xf5, 0xee, 0xe7,
	    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04, 0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
	    0xab, 0xa2, 0xb9, 0xb0, 0x8f, 0x86, 0x9d, 0x94, 0xe3, 0xea, 0xf1, 0xf8, 0xc7, 0xce, 0xd5, 0xdc,
	    0x76, 0x7f, 0x64, 0x6d, 0x52, 0x5b, 0x40, 0x49, 0x3e, 0x37, 0x2c, 0x25, 0x1a, 0x13, 0x08, 0x01,
	    0xe6, 0xef, 0xf4, 0xfd, 0xc2, 0xcb, 0xd0, 0xd9, 0xae, 0xa7, 0xbc, 0xb5, 0x8a, 0x83, 0x98, 0x91,
	    0x4d, 0x44, 0x5f, 0x56, 0x69, 0x60, 0x7b, 0x72, 0x05, 0x0c, 0x17, 0x1e, 0x21, 0x28, 0x33, 0x3a,
	    0xdd, 0xd4, 0xcf, 0xc6, 0xf9, 0xf0, 0xeb, 0xe2, 0x95, 0x9c, 0x87, 0x8e, 0xb1, 0xb8, 0xa3, 0xaa,
	    0xec, 0xe5, 0xfe, 0xf7, 0xc8, 0xc1, 0xda, 0xd3, 0xa4, 0xad, 0xb6, 0xbf, 0x80, 0x89, 0x92, 0x9b,
	    0x7c, 0x75, 0x6e, 0x67, 0x58, 0x51, 0x4a, 0x43, 0x34, 0x3d, 0x26, 0x2f, 0x10, 0x19, 0x02, 0x0b,
	    0xd7, 0xde, 0xc5, 0xcc, 0xf3, 0xfa, 0xe1, 0xe8, 0x9f, 0x96, 0x8d, 0x84, 0xbb, 0xb2, 0xa9, 0xa0,
	    0x47, 0x4e, 0x55, 0x5c, 0x63, 0x6a, 0x71, 0x78, 0x0f, 0x06, 0x1d, 0x14, 0x2b, 0x22, 0x39, 0x30,
	    0x9a, 0x93, 0x88, 0x81, 0xbe, 0xb7, 0xac, 0xa5, 0xd2, 0xdb, 0xc0, 0xc9, 0xf6, 0xff, 0xe4, 0xed,
	    0x0a, 0x03, 0x18, 0x11, 0x2e, 0x27, 0x3c, 0x35, 0x42, 0x4b, 0x50, 0x59, 0x66, 0x6f, 0x74, 0x7d,
	    0xa1, 0xa8, 0xb3, 0xba, 0x85, 0x8c, 0x97, 0x9e, 0xe9, 0xe0, 0xfb, 0xf2, 0xcd, 0xc4, 0xdf, 0xd6,
	    0x31, 0x38, 0x23, 0x2a, 0x15, 0x1c, 0x07, 0x0e, 0x79, 0x70, 0x6b, 0x62, 0x5d, 0x54, 0x4f, 0x46
	};

	static const unsigned char mulBtbl[256] = {
	    0x00, 0x0b, 0x16, 0x1d, 0x2c, 0x27, 0x3a, 0x31, 0x58, 0x53, 0x4e, 0x45, 0x74, 0x7f, 0x62, 0x69,
	    0xb0, 0xbb, 0xa6, 0xad, 0x9c, 0x97, 0x8a, 0x81, 0xe8, 0xe3, 0xfe, 0xf5, 0xc4, 0xcf, 0xd2, 0xd9,
	    0x7b, 0x70, 0x6d, 0x66, 0x57, 0x5c, 0x41, 0x4a, 0x23, 0x28, 0x35, 0x3e, 0x0f, 0x04, 0x19, 0x12,
	    0xcb, 0xc0, 0xdd, 0xd6, 0xe7, 0xec, 0xf1, 0xfa, 0x93, 0x98, 0x85, 0x8e, 0xbf, 0xb4, 0xa9, 0xa2,
	    0xf6, 0xfd, 0xe0, 0xeb, 0xda, 0xd1, 0xcc, 0xc7, 0xae, 0xa5, 0xb8, 0xb3, 0x82, 0x89, 0x94, 0x9f,
	    0x46, 0x4d, 0x50, 0x5b, 0x6a, 0x61, 0x7c, 0x77, 0x1e, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2f,
	    0x8d, 0x86, 0x9b, 0x90, 0xa1, 0xaa, 0xb7, 0xbc, 0xd5, 0xde, 0xc3, 0xc8, 0xf9, 0xf2, 0xef, 0xe4,
	    0x3d, 0x36, 0x2b, 0x20, 0x11, 0x1a, 0x07, 0x0c, 0x65, 0x6e, 0x73, 0x78, 0x49, 0x42, 0x5f, 0x54,
	    0xf7, 0xfc, 0xe1, 0xea, 0xdb, 0xd0, 0xcd, 0xc6, 0xaf, 0xa4, 0xb9, 0xb2, 0x83, 0x88, 0x95, 0x9e,
	    0x47, 0x4c, 0x51, 0x5a, 0x6b, 0x60, 0x7d, 0x76, 0x1f, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2e,
	    0x8c, 0x87, 0x9a, 0x91, 0xa0, 0xab, 0xb6, 0xbd, 0xd4, 0xdf, 0xc2, 0xc9, 0xf8, 0xf3, 0xee, 0xe5,
	    0x3c, 0x37, 0x2a, 0x21, 0x10, 0x1b, 0x06, 0x0d, 0x64, 0x6f, 0x72, 0x79, 0x48, 0x43, 0x5e, 0x55,
	    0x01, 0x0a, 0x17, 0x1c, 0x2d, 0x26, 0x3b, 0x30, 0x59, 0x52, 0x4f, 0x44, 0x75, 0x7e, 0x63, 0x68,
	    0xb1, 0xba, 0xa7, 0xac, 0x9d, 0x96, 0x8b, 0x80, 0xe9, 0xe2, 0xff, 0xf4, 0xc5, 0xce, 0xd3, 0xd8,
	    0x7a, 0x71, 0x6c, 0x67, 0x56, 0x5d, 0x40, 0x4b, 0x22, 0x29, 0x34, 0x3f, 0x0e, 0x05, 0x18, 0x13,
	    0xca, 0xc1, 0xdc, 0xd7, 0xe6, 0xed, 0xf0, 0xfb, 0x92, 0x99, 0x84, 0x8f, 0xbe, 0xb5, 0xa8, 0xa3
	};

	static const unsigned char mulDtbl[256] = {
	    0x00, 0x0d, 0x1a, 0x17, 0x34, 0x39, 0x2e, 0x23, 0x68, 0x65, 0x72, 0x7f, 0x5c, 0x51, 0x46, 0x4b,
	    0xd0, 0xdd, 0xca, 0xc7, 0xe4, 0xe9, 0xfe, 0xf3, 0xb8, 0xb5, 0xa2, 0xaf, 0x8c, 0x81, 0x96, 0x9b,
	    0xbb, 0xb6, 0xa1, 0xac, 0x8f, 0x82, 0x95, 0x98, 0xd3, 0xde, 0xc9, 0xc4, 0xe7, 0xea, 0xfd, 0xf0,
	    0x6b, 0x66, 0x71, 0x7c, 0x5f, 0x52, 0x45, 0x48, 0x03, 0x0e, 0x19, 0x14, 0x37, 0x3a, 0x2d, 0x20,
	    0x6d, 0x60, 0x77, 0x7a, 0x59, 0x54, 0x43, 0x4e, 0x05, 0x08, 0x1f, 0x12, 0x31, 0x3c, 0x2b, 0x26,
	    0xbd, 0xb0, 0xa7, 0xaa, 0x89, 0x84, 0x93, 0x9e, 0xd5, 0xd8, 0xcf, 0xc2, 0xe1, 0xec, 0xfb, 0xf6,
	    0xd6, 0xdb, 0xcc, 0xc1, 0xe2, 0xef, 0xf8, 0xf5, 0xbe, 0xb3, 0xa4, 0xa9, 0x8a, 0x87, 0x90, 0x9d,
	    0x06, 0x0b, 0x1c, 0x11, 0x32, 0x3f, 0x28, 0x25, 0x6e, 0x63, 0x74, 0x79, 0x5a, 0x57, 0x40, 0x4d,
	    0xda, 0xd7, 0xc0, 0xcd, 0xee, 0xe3, 0xf4, 0xf9, 0xb2, 0xbf, 0xa8, 0xa5, 0x86, 0x8b, 0x9c, 0x91,
	    0x0a, 0x07, 0x10, 0x1d, 0x3e, 0x33, 0x24, 0x29, 0x62, 0x6f, 0x78, 0x75, 0x56, 0x5b, 0x4c, 0x41,
	    0x61, 0x6c, 0x7b, 0x76, 0x55, 0x58, 0x4f, 0x42, 0x09, 0x04, 0x13, 0x1e, 0x3d, 0x30, 0x27, 0x2a,
	    0xb1, 0xbc, 0xab, 0xa6, 0x85, 0x88, 0x9f, 0x92, 0xd9, 0xd4, 0xc3, 0xce, 0xed, 0xe0, 0xf7, 0xfa,
	    0xb7, 0xba, 0xad, 0xa0, 0x83, 0x8e, 0x99, 0x94, 0xdf, 0xd2, 0xc5, 0xc8, 0xeb, 0xe6, 0xf1, 0xfc,
	    0x67, 0x6a, 0x7d, 0x70, 0x53, 0x5e, 0x49, 0x44, 0x0f, 0x02, 0x15, 0x18, 0x3b, 0x36, 0x21, 0x2c,
	    0x0c, 0x01, 0x16, 0x1b, 0x38, 0x35, 0x22, 0x2f, 0x64, 0x69, 0x7e, 0x73, 0x50, 0x5d, 0x4a, 0x47,
	    0xdc, 0xd1, 0xc6, 0xcb, 0xe8, 0xe5, 0xf2, 0xff, 0xb4, 0xb9, 0xae, 0xa3, 0x80, 0x8d, 0x9a, 0x97
	};

	static const unsigned char mulEtbl[256] = {
	    0x00, 0x0e, 0x1c, 0x12, 0x38, 0x36, 0x24, 0x2a, 0x70, 0x7e, 0x6c, 0x62, 0x48, 0x46, 0x54, 0x5a,
	    0xe0, 0xee, 0xfc, 0xf2, 0xd8, 0xd6, 0xc4, 0xca, 0x90, 0x9e, 0x8c, 0x82, 0xa8, 0xa6, 0xb4, 0xba,
	    0xdb, 0xd5, 0xc7, 0xc9, 0xe3, 0xed, 0xff, 0xf1, 0xab, 0xa5, 0xb7, 0xb9, 0x93, 0x9d, 0x8f, 0x81,
	    0x3b, 0x35, 0x27, 0x29, 0x03, 0x0d, 0x1f, 0x11, 0x4b, 0x45, 0x57, 0x59, 0x73, 0x7d, 0x6f, 0x61,
	    0xad, 0xa3, 0xb1, 0xbf, 0x95, 0x9b, 0x89, 0x87, 0xdd, 0xd3, 0xc1, 0xcf, 0xe5, 0xeb, 0xf9, 0xf7,
	    0x4d, 0x43, 0x51, 0x5f, 0x75, 0x7b, 0x69, 0x67, 0x3d, 0x33, 0x21, 0x2f, 0x05, 0x0b, 0x19, 0x17,
	    0x76, 0x78, 0x6a, 0x64, 0x4e, 0x40, 0x52, 0x5c, 0x06, 0x08, 0x1a, 0x14, 0x3e, 0x30, 0x22, 0x2c,
	    0x96, 0x98, 0x8a, 0x84, 0xae, 0xa0, 0xb2, 0xbc, 0xe6, 0xe8, 0xfa, 0xf4, 0xde, 0xd0, 0xc2, 0xcc,
	    0x41, 0x4f, 0x5d, 0x53, 0x79, 0x77, 0x65, 0x6b, 0x31, 0x3f, 0x2d, 0x23, 0x09, 0x07, 0x15, 0x1b,
	    0xa1, 0xaf, 0xbd, 0xb3, 0x99, 0x97, 0x85, 0x8b, 0xd1, 0xdf, 0xcd, 0xc3, 0xe9, 0xe7, 0xf5, 0xfb,
	    0x9a, 0x94, 0x86, 0x88, 0xa2, 0xac, 0xbe, 0xb0, 0xea, 0xe4, 0xf6, 0xf8, 0xd2, 0xdc, 0xce, 0xc0,
	    0x7a, 0x74, 0x66, 0x68, 0x42, 0x4c, 0x5e, 0x50, 0x0a, 0x04, 0x16, 0x18, 0x32, 0x3c, 0x2e, 0x20,
	    0xec, 0xe2, 0xf0, 0xfe, 0xd4, 0xda, 0xc8, 0xc6, 0x9c, 0x92, 0x80, 0x8e, 0xa4, 0xaa, 0xb8, 0xb6,
	    0x0c, 0x02, 0x10, 0x1e, 0x34, 0x3a, 0x28, 0x26, 0x7c, 0x72, 0x60, 0x6e, 0x44, 0x4a, 0x58, 0x56,
	    0x37, 0x39, 0x2b, 0x25, 0x0f, 0x01, 0x13, 0x1d, 0x47, 0x49, 0x5b, 0x55, 0x7f, 0x71, 0x63, 0x6d,
	    0xd7, 0xd9, 0xcb, 0xc5, 0xef, 0xe1, 0xf3, 0xfd, 0xa7, 0xa9, 0xbb, 0xb5, 0x9f, 0x91, 0x83, 0x8d
	};

	static const unsigned char shiftTbl[16] = {
		0, 13, 10, 7, 4, 1, 14, 11, 8, 5, 2, 15, 12, 9, 6, 3
	};

	static const unsigned char keyXorpad[16] = {
		0xa2, 0x25, 0x25, 0x99, 0xb7, 0x62, 0xf4, 0x39, 0x28, 0xe1, 0xb7, 0x73, 0x91, 0x05, 0x25, 0x87
	};

	static void xorRoundKey(unsigned char key[16], const unsigned char roundKey[16]) {
		unsigned int i;
		for (i = 0; i < 16; i++) {
			key[i] ^= roundKey[i];
		}
	}

	static void subBytes(unsigned char key[16]) {
		unsigned int i;
		for (i = 0; i < 16; i++) {
			key[i] = sbox[key[i]];
		}
	}

	static void shiftRows(unsigned char key[16]) {
		unsigned int i;
		char key2[16];
		memcpy(key2, key, 16);
		for (i = 0; i < 16; i++) {
			key[i] = key2[shiftTbl[i]];
		}
	}

	static void mixCol(unsigned char key[4]) {
		unsigned char a0 = key[0];
		unsigned char a1 = key[1];
		unsigned char a2 = key[2];
		unsigned char a3 = key[3];
		key[0] = mulEtbl[a0] ^ mul9tbl[a3] ^ mulDtbl[a2] ^ mulBtbl[a1];
		key[1] = mulEtbl[a1] ^ mul9tbl[a0] ^ mulDtbl[a3] ^ mulBtbl[a2];
		key[2] = mulEtbl[a2] ^ mul9tbl[a1] ^ mulDtbl[a0] ^ mulBtbl[a3];
		key[3] = mulEtbl[a3] ^ mul9tbl[a2] ^ mulDtbl[a1] ^ mulBtbl[a0];
	}

	static void mixCols(unsigned char _key[16]) {
		unsigned char key[4][4];
		memcpy(key, _key, 16);
		unsigned int i;
		for (i = 0; i < 4; i++) {
			mixCol(key[i]);
		}
		memcpy(_key, key, 16);
	}

	// this implements a subset of the AES algorithm
	static void scrambleKey(unsigned char key[16]) {
		xorRoundKey(key, aesRoundKeys[0]);
		unsigned int i;
		for (i = 1; i < 10; i++) {
			subBytes(key);
			shiftRows(key);
			mixCols(key);
			xorRoundKey(key, aesRoundKeys[i]);
		}
		subBytes(key);
		shiftRows(key);
		xorRoundKey(key, aesRoundKeys[10]);
		for (i = 0; i < 16; i++) {
			key[i] ^= keyXorpad[i];
		}
	}

	void genXorpadFromSeed(unsigned long long seed, unsigned char* _xorpad, size_t sz) {
		if (_xorpad == NULL) return;
		unsigned int i;
		unsigned long long* xorpad = (unsigned long long*) _xorpad;
		init_genrand64(seed);
		for (i = 0; i < (sz / sizeof(long long)); i++) {
			xorpad[i] = genrand64_int64();
		}
	}

	void genXorpadFromSeed2(unsigned long long seed, unsigned char* _xorpad, size_t sz) {
		if (_xorpad == NULL) return;
		unsigned int i;
		unsigned long long* xorpad = (unsigned long long*) _xorpad;
		init_genrand64(seed);
		//init_genrand64(genrand64_int64());
		//genrand64_int64();
		for (i = 0; i < (sz / sizeof(long long)); i++) {
			xorpad[i] = htobe64(genrand64_int64());
		}
	}
}
