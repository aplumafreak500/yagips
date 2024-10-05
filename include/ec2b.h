/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef EC2B_H
#define EC2B_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	unsigned int magic;
	unsigned int keyLen;
	unsigned char key[16];
	unsigned int dataLen;
	unsigned char data[2048];
} ec2b_t;

void genXorpadFromSeed(unsigned long long, unsigned char*, size_t);
void genXorpadFromSeed2(unsigned long long, unsigned char*, size_t);
#ifdef __cplusplus
}

#include <stdio.h>
#include <string>

class Ec2b {
public:
	Ec2b(); // default, key and seed left uninitalized
	Ec2b(int); // Key and seed either randomly generated or zero-filled based on arg
	Ec2b(unsigned long long); // xor derived from given mt seed; key and seed are generated randomly
	Ec2b(unsigned long long, int); // xor derived from given mt seed; key and seed are set based on second arg
	Ec2b(const char*); // Key and seed in file, path given in arg
	Ec2b(FILE*); // Key and seed in file, caller is responsible for closing it. Seek position is preserved
	Ec2b(const std::string&); // Key and seed as ec2b struct
	Ec2b(const char*, size_t); // as above but with c-style types
	Ec2b(ec2b_t); // Key and seed already stored as type ec2b_t
	Ec2b(const ec2b_t*); // as above but takes a pointer
	Ec2b(const std::string&, const std::string&); // Key and seed in params
	Ec2b(const char*, size_t, const char*, size_t); // as above but with c-style types
	~Ec2b();
	const std::string& getKey() const;
	void setKey(const std::string&);
	const std::string& getData() const;
	void setData(const std::string&);
	const std::string& getXorpad() const;
	unsigned long long getSeed() const;
	void setSeed(unsigned long long);
	void setSeed(unsigned long long, int);
	operator std::string() const;
	operator ec2b_t() const;
	enum {
		SEED_FROM_OBJECT = 0,
		SEED_FROM_RAND,
		SEED_FROM_SEED,
		SEED_FROM_SEED_MT,
		SEED_FROM_SEED_XOR,
		SEED_FROM_ZERO,
		SEED_FROM_CONST_HIGH_BYTE
	};
private:
	std::string key;
	std::string data;
	std::string xorpad;
	unsigned long long seed;
	void deriveXor();
	void getFromSeed(int);
};
#endif
#endif
