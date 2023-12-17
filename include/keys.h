/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef KEYS_H
#define KEYS_H
#ifdef __cplusplus
extern "C" {
#endif
extern const unsigned char dispatchKey[4096];
extern const unsigned char dispatchSeed[2076];
extern const unsigned char secretKey[4096];
extern const unsigned char secretKeyBuf[32];
extern const unsigned char authPrivKey[608];
extern const unsigned char signPrivKey[1193];
extern const unsigned char* const DispatchKeys[6];
#ifdef __cplusplus
}
#endif
#endif
