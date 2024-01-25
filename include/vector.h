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
