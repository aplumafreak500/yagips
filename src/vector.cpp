/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "vector.h"
#include "define.pb.h"

Vector::Vector() {}

Vector::Vector(const proto::Vector& pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

Vector::Vector(const proto::Vector3Int& pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

Vector::Vector(const VectorInt& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector::operator proto::Vector() const {
	proto::Vector pb;
	pb.set_x(x);
	pb.set_y(y);
	pb.set_z(z);
	return pb;
}

Vector::operator proto::Vector3Int() const {
	proto::Vector3Int pb;
	pb.set_x(x);
	pb.set_y(y);
	pb.set_z(z);
	return pb;
}

VectorInt::VectorInt() {}

VectorInt::VectorInt(const proto::Vector& pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

VectorInt::VectorInt(const proto::Vector3Int& pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

VectorInt::VectorInt(const Vector& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

VectorInt::operator proto::Vector() const {
	proto::Vector pb;
	pb.set_x(x);
	pb.set_y(y);
	pb.set_z(z);
	return pb;
}

VectorInt::operator proto::Vector3Int() const {
	proto::Vector3Int pb;
	pb.set_x(x);
	pb.set_y(y);
	pb.set_z(z);
	return pb;
}
