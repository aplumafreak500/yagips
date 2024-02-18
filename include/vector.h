/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef VECTOR_H
#define VECTOR_H
#include "define.pb.h"
struct Vector;
struct VectorInt;

struct Vector {
	long double x;
	long double y;
	long double z;
	Vector();
	Vector(proto::Vector);
	Vector(proto::Vector3Int);
	Vector(VectorInt);
	operator proto::Vector() const;
	operator proto::Vector3Int() const;
	operator VectorInt() const;
};

struct VectorInt {
	long long x;
	long long y;
	long long z;
	VectorInt();
	VectorInt(proto::Vector);
	VectorInt(proto::Vector3Int);
	VectorInt(Vector);
	operator proto::Vector() const;
	operator proto::Vector3Int() const;
	operator Vector() const;
};
#endif
