/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef CRYPT_H
#define CRYPT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int HyvCryptRsaEnc(const unsigned char*, size_t, unsigned char*, size_t, unsigned int);
int HyvCryptRsaSign(const unsigned char*, size_t, unsigned char*, size_t);
int HyvCryptXor(unsigned char*, size_t, const unsigned char*, size_t);
#ifdef __cplusplus
}
#endif
#endif
