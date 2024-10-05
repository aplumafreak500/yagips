/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <string>
#include "util.h"

std::string b64enc(const std::string& in) {
	const char b64tbl[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
	};
	std::string ret = "";
	size_t isz = in.size();
	unsigned int i = 0;
	unsigned int t;
	while (isz > 0) {
		t = (in[0 + i] >> 2) & 0x3f;
		ret += b64tbl[t];
		t = in[0 + i] << 4;
		isz--;
		if (isz) {
			t |= in[1 + i] >> 4;
		}
		t &= 0x3f;
		ret += b64tbl[t];
		if (!isz) {
			ret += '=';
		}
		else {
			t = in[1 + i] << 2;
			isz--;
			if (isz) {
				t |= in[2 + i] >> 6;
			}
			t &= 0x3f;
			ret += b64tbl[t];
		}
		if (!isz) {
			ret += '=';
		}
		else {
			t = in[2 + i] & 0x3f;
			ret += b64tbl[t];
			isz--;
			i += 3;
		}
	}
	return ret;
}

std::string b64dec(const std::string& in) {
	std::string ret = "";
	const int b64itbl[128] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
	};
	size_t isz = in.size();
	unsigned int i = 0;
	unsigned char t;
	unsigned char e;
	while (isz > 0) {
idx0:
		t = in[i + 0];
		if (t > 128) {
			i++;
			isz--;
			if (!isz) break;
			goto idx0;
		}
		if (b64itbl[t] == -1) {
			i++;
			isz--;
			if (!isz) break;
			goto idx0;
		}
		e = (b64itbl[t] & 0x3f) << 2;
		isz--;
		if (!isz) {
			ret += e;
			break;
		}
idx1:
		t = in[i + 1];
		if (t > 128) {
			i++;
			isz--;
			if (!isz) break;
			goto idx1;
		}
		if (b64itbl[t] == -1) {
			i++;
			isz--;
			goto idx1;
		}
		e |= (b64itbl[t] & 0x3f) >> 4;
		ret += e;
		e = (b64itbl[t] & 0x3f) << 4;
		isz--;
		if (!isz) {
			ret += e;
			break;
		}
idx2:
		t = in[i + 2];
		if (t > 128) {
			i++;
			isz--;
			if (!isz) break;
			goto idx2;
		}
		if (b64itbl[t] == -1) {
			i++;
			isz--;
			if (!isz) break;
			goto idx2;
		}
		e |= (b64itbl[t] & 0x3f) >> 2;
		ret += e;
		e = (b64itbl[t] & 0x3f) << 6;
		isz--;
		if (!isz) {
			ret += e;
			break;
		}
idx3:
		t = in[i + 3];
		if (t > 128) {
			i++;
			isz--;
			if (!isz) break;
			goto idx3;
		}
		if (b64itbl[t] == -1) {
			i++;
			isz--;
			if (!isz) break;
			goto idx3;
		}
		e |= (b64itbl[t] & 0x3f);
		ret += e;
		i += 4;
		isz--;
	}
	return ret;
}

std::string hexenc(const std::string& in) {
	std::string ret = "";
	size_t isz = in.size();
	unsigned int i = 0;
	unsigned char t;
	while (isz > 0) {
		t = (in[i] >> 4) & 0xf;
		if (t + '0' > '9') {
			ret += (t - 10 + 'a');
		}
		else {
			ret += (t + '0');
		}
		t = in[i] & 0xf;
		if (t + '0' > '9') {
			ret += (t - 10 + 'a');
		}
		else {
			ret += (t + '0');
		}
		i++;
		isz--;
	}
	return ret;
}

std::string hexdec(const std::string& in) {
	std::string ret = "";
	size_t isz = in.size();
	unsigned int i = 0;
	unsigned char t;
	unsigned char e;
	while (isz > 0) {
idx0:
		t = in[i + 0];
		if (t < '0' || t > 'f') {
			i++;
			isz--;
			if (!isz) break;
			goto idx0;
		}
		if (t >= 'a') t -= 0x20;
		if (t > 'F') {
			i++;
			isz--;
			if (!isz) break;
			goto idx0;
		}
		if (t > '9') t -= 7;
		t -= '0';
		if (t > 16) {
			i++;
			isz--;
			if (!isz) break;
			goto idx0;
		}
		e = t << 4;
		isz--;
		if (!isz) {
			ret += e;
			break;
		}
idx1:
		t = in[i + 1];
		if (t < '0' || t > 'f') {
			i++;
			isz--;
			if (!isz) break;
			goto idx1;
		}
		if (t >= 'a') t -= 0x20;
		if (t > 'F') {
			i++;
			isz--;
			if (!isz) break;
			goto idx1;
		}
		if (t > '9') t -= 7;
		t -= '0';
		if (t > 16) {
			i++;
			isz--;
			if (!isz) break;
			goto idx1;
		}
		e |= t;
		ret += e;
		i += 2;
		isz--;
	}
	return ret;
}

unsigned int toInt(const std::string& x) {
	return strtoul(x.c_str(), NULL, 0);
}

long double toFlt(const std::string& x) {
	return strtold(x.c_str(), NULL);
}

extern "C" {
	void DbgHexdump(const unsigned char* buf, size_t sz) {
		size_t off;
		unsigned char abuf[17];
		abuf[16] = '\0';
		if (buf == NULL) return;
		for (off = 0; off < sz; off++) {
			if (off % 16 == 0) {
				if (off != 0) {
					fprintf(stderr, " %s\n", abuf);
				}
				fprintf(stderr, "%08lx: ", off);
			}
			fprintf(stderr, "%02x ", buf[off]);
			if (buf[off] < ' ' || buf[off] > '~') {
				abuf[off % 16] = '.';
			}
			else {
				abuf[off % 16] = buf[off];
			}
		}
		abuf[off % 16] = '\0';
		for (off %=16; off < 16; off++) {
			fprintf(stderr, "   ");
		}
		fprintf(stderr, "  %s\n", abuf);
	}

	#define rotl(x, k) ((x << k) | (x >> (64 - k)))

	unsigned long long rand_xoshiro256(unsigned long long s[4]) {
		assert(s != NULL);
		assert((s[0] | s[1] | s[2] | s[3]) != 0);
		unsigned long long result = rotl((s[0] + s[3]), 23) + s[0];
		unsigned long long t = s[1] << 17;
		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];
		s[2] ^= t;
		s[3] = rotl(s[3], 45);
		return result;
	}

	unsigned long long curTimeMs() {
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
		return (t.tv_sec * 1000) + (t.tv_nsec / 1000000);
	}

}
