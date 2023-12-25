/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
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
	int t;
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

extern "C" {
	void DbgHexdump(const unsigned char* buf, size_t sz) {
		size_t off;
		unsigned char abuf[17];
		abuf[16] = '\0';
		if (buf == NULL) return;
		for (off = 0; off < sz; off++) {
			if (off % 16 == 0) {
				if (off != 0) {
					fprintf(stderr, "\t%s\n", abuf);
				}
				fprintf(stderr, "%04lx \t", off);
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
		fprintf(stderr, "\t%s\n", abuf);
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

}
