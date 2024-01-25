#include "vector.h"
#include "define.pb.h"

Vector::Vector() {}

Vector::Vector(proto::Vector pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

Vector::Vector(proto::Vector3Int pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

Vector::Vector(VectorInt v) {
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

Vector::operator VectorInt() const {
	return VectorInt(*this);
}

VectorInt::VectorInt() {}

VectorInt::VectorInt(proto::Vector pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

VectorInt::VectorInt(proto::Vector3Int pb) {
	x = pb.x();
	y = pb.y();
	z = pb.z();
}

VectorInt::VectorInt(Vector v) {
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

VectorInt::operator Vector() const {
	return VectorInt(*this);
}

