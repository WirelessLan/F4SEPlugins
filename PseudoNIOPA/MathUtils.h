#pragma once
#include <cmath>
namespace RE {
	class NiPoint3 {
	public:
		using value_type = float;
		using size_type = std::size_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		[[nodiscard]] reference operator[](size_type a_pos) noexcept {
			return reinterpret_cast<pointer>(std::addressof(x))[a_pos];
		}

		[[nodiscard]] const_reference operator[](size_type a_pos) const noexcept {
			return reinterpret_cast<const_pointer>(std::addressof(x))[a_pos];
		}

		NiPoint3(float _x = 0.f, float _y = 0.f, float _z = 0.f) {
			x = _x;
			y = _y;
			z = _z;
		}

		// members
		value_type x{ 0.0F };  // 0
		value_type y{ 0.0F };  // 4
		value_type z{ 0.0F };  // 8

		NiPoint3 operator+ (const NiPoint3& pt) const {
			return NiPoint3(x + pt.x, y + pt.y, z + pt.z);
		}

		NiPoint3 operator- (const NiPoint3& pt) const {
			return NiPoint3(x - pt.x, y - pt.y, z - pt.z);
		}

		NiPoint3 operator* (float s) const {
			return NiPoint3(x * s, y * s, z * s);
		}

		NiPoint3 operator/ (float s) const {
			return NiPoint3(x / s, y / s, z / s);
		}
	};

	class NiPoint4 {
	public:
		struct NiPoint4Struct {
		public:
			// members
			float x;  // 00
			float y;  // 04
			float z;  // 08
			float w;  // 0C
		};

		// members
		union {
			NiPoint4Struct v;
			float pt[4]{ 0.0F };
		};  // 00
	};

	class alignas(0x10) NiMatrix3 {
	public:
		void MakeIdentity() noexcept {
			entry[0].v = { 1.0F, 0.0F, 0.0F, 0.0F };
			entry[1].v = { 0.0F, 1.0F, 0.0F, 0.0F };
			entry[2].v = { 0.0F, 0.0F, 1.0F, 0.0F };
		}

		// members
		NiPoint4 entry[3];  // 00

		NiMatrix3 operator* (const NiMatrix3& rhs) const {
			NiMatrix3 tmp;
			tmp.entry[0].pt[0] =
				entry[0].pt[0] * rhs.entry[0].pt[0] +
				entry[0].pt[1] * rhs.entry[1].pt[0] +
				entry[0].pt[2] * rhs.entry[2].pt[0];
			tmp.entry[1].pt[0] =
				entry[1].pt[0] * rhs.entry[0].pt[0] +
				entry[1].pt[1] * rhs.entry[1].pt[0] +
				entry[1].pt[2] * rhs.entry[2].pt[0];
			tmp.entry[2].pt[0] =
				entry[2].pt[0] * rhs.entry[0].pt[0] +
				entry[2].pt[1] * rhs.entry[1].pt[0] +
				entry[2].pt[2] * rhs.entry[2].pt[0];
			tmp.entry[0].pt[1] =
				entry[0].pt[0] * rhs.entry[0].pt[1] +
				entry[0].pt[1] * rhs.entry[1].pt[1] +
				entry[0].pt[2] * rhs.entry[2].pt[1];
			tmp.entry[1].pt[1] =
				entry[1].pt[0] * rhs.entry[0].pt[1] +
				entry[1].pt[1] * rhs.entry[1].pt[1] +
				entry[1].pt[2] * rhs.entry[2].pt[1];
			tmp.entry[2].pt[1] =
				entry[2].pt[0] * rhs.entry[0].pt[1] +
				entry[2].pt[1] * rhs.entry[1].pt[1] +
				entry[2].pt[2] * rhs.entry[2].pt[1];
			tmp.entry[0].pt[2] =
				entry[0].pt[0] * rhs.entry[0].pt[2] +
				entry[0].pt[1] * rhs.entry[1].pt[2] +
				entry[0].pt[2] * rhs.entry[2].pt[2];
			tmp.entry[1].pt[2] =
				entry[1].pt[0] * rhs.entry[0].pt[2] +
				entry[1].pt[1] * rhs.entry[1].pt[2] +
				entry[1].pt[2] * rhs.entry[2].pt[2];
			tmp.entry[2].pt[2] =
				entry[2].pt[0] * rhs.entry[0].pt[2] +
				entry[2].pt[1] * rhs.entry[1].pt[2] +
				entry[2].pt[2] * rhs.entry[2].pt[2];
			return tmp;
		}

		NiMatrix3 operator* (float scalar) const {
			NiMatrix3 result;
			result.entry[0].pt[0] = entry[0].pt[0] * scalar;
			result.entry[0].pt[1] = entry[0].pt[1] * scalar;
			result.entry[0].pt[2] = entry[0].pt[2] * scalar;
			result.entry[1].pt[0] = entry[1].pt[0] * scalar;
			result.entry[1].pt[1] = entry[1].pt[1] * scalar;
			result.entry[1].pt[2] = entry[1].pt[2] * scalar;
			result.entry[2].pt[0] = entry[2].pt[0] * scalar;
			result.entry[2].pt[1] = entry[2].pt[1] * scalar;
			result.entry[2].pt[2] = entry[2].pt[2] * scalar;
			return result;
		}

		NiPoint3 operator* (const NiPoint3& p) const {
			return NiPoint3
			(
				entry[0].pt[0] * p.x + entry[0].pt[1] * p.y + entry[0].pt[2] * p.z,
				entry[1].pt[0] * p.x + entry[1].pt[1] * p.y + entry[1].pt[2] * p.z,
				entry[2].pt[0] * p.x + entry[2].pt[1] * p.y + entry[2].pt[2] * p.z
			);
		}
	};
}

using NiMatrix3CLib = RE::NiMatrix3;
using NiPoint3CLib = RE::NiPoint3;

struct Quaternion {
public:
	float x, y, z, w;
	Quaternion(float _x, float _y, float _z, float _w);
	float Norm();
	NiMatrix3CLib ToRotationMatrix33();
};

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

float Quaternion::Norm() {
	return x * x + y * y + z * z + w * w;
}

void SetMatrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i, NiMatrix3CLib& mat) {
	mat.entry[0].pt[0] = a;
	mat.entry[0].pt[1] = b;
	mat.entry[0].pt[2] = c;
	mat.entry[1].pt[0] = d;
	mat.entry[1].pt[1] = e;
	mat.entry[1].pt[2] = f;
	mat.entry[2].pt[0] = g;
	mat.entry[2].pt[1] = h;
	mat.entry[2].pt[2] = i;
}

NiMatrix3CLib GetRotationMatrix33(float pitch, float yaw, float roll) {
	NiMatrix3CLib m_yaw;
	SetMatrix33(cos(yaw), -sin(yaw), 0,
				sin(yaw), cos(yaw), 0,
				0, 0, 1,
				m_yaw);
	NiMatrix3CLib m_roll;
	SetMatrix33(1, 0, 0,
				0, cos(roll), -sin(roll),
				0, sin(roll), cos(roll),
				m_roll);
	NiMatrix3CLib m_pitch;
	SetMatrix33(cos(pitch), 0, sin(pitch),
				0, 1, 0,
				-sin(pitch), 0, cos(pitch),
				m_pitch);
	return m_roll * m_pitch * m_yaw;
}

NiMatrix3CLib GetRotationMatrix33(NiPoint3CLib axis, float angle) {
	float x = axis.x * sin(angle / 2.0f);
	float y = axis.y * sin(angle / 2.0f);
	float z = axis.z * sin(angle / 2.0f);
	float w = cos(angle / 2.0f);
	Quaternion q = Quaternion(x, y, z, w);
	return q.ToRotationMatrix33();
}

//From https://android.googlesource.com/platform/external/jmonkeyengine/+/59b2e6871c65f58fdad78cd7229c292f6a177578/engine/src/core/com/jme3/math/Quaternion.java
NiMatrix3CLib Quaternion::ToRotationMatrix33() {
	float norm = Norm();
	// we explicitly test norm against one here, saving a division
	// at the cost of a test and branch.  Is it worth it?
	float s = (norm == 1.0f) ? 2.0f : (norm > 0.0f) ? 2.0f / norm : 0;

	// compute xs/ys/zs first to save 6 multiplications, since xs/ys/zs
	// will be used 2-4 times each.
	float xs = x * s;
	float ys = y * s;
	float zs = z * s;
	float xx = x * xs;
	float xy = x * ys;
	float xz = x * zs;
	float xw = w * xs;
	float yy = y * ys;
	float yz = y * zs;
	float yw = w * ys;
	float zz = z * zs;
	float zw = w * zs;

	// using s=2/norm (instead of 1/norm) saves 9 multiplications by 2 here
	NiMatrix3CLib mat;
	SetMatrix33(1 - (yy + zz), (xy - zw), (xz + yw),
				(xy + zw), 1 - (xx + zz), (yz - xw),
				(xz - yw), (yz + xw), 1 - (xx + yy),
				mat);

	return mat;
}

//Sarrus rule
float Determinant(NiMatrix3CLib mat) {
	return mat.entry[0].pt[0] * mat.entry[1].pt[1] * mat.entry[2].pt[2]
		+ mat.entry[0].pt[1] * mat.entry[1].pt[2] * mat.entry[2].pt[0]
		+ mat.entry[0].pt[2] * mat.entry[1].pt[0] * mat.entry[2].pt[1]
		- mat.entry[0].pt[2] * mat.entry[1].pt[1] * mat.entry[2].pt[0]
		- mat.entry[0].pt[1] * mat.entry[1].pt[0] * mat.entry[2].pt[2]
		- mat.entry[0].pt[0] * mat.entry[1].pt[2] * mat.entry[2].pt[1];
}

NiMatrix3CLib Inverse(NiMatrix3CLib mat) {
	float det = Determinant(mat);
	if (det == 0) {
		NiMatrix3CLib idmat;
		idmat.MakeIdentity();
		return idmat;
	}
	float a = mat.entry[0].pt[0];
	float b = mat.entry[0].pt[1];
	float c = mat.entry[0].pt[2];
	float d = mat.entry[1].pt[0];
	float e = mat.entry[1].pt[1];
	float f = mat.entry[1].pt[2];
	float g = mat.entry[2].pt[0];
	float h = mat.entry[2].pt[1];
	float i = mat.entry[2].pt[2];
	NiMatrix3CLib invmat;
	SetMatrix33(e * i - f * h, -(b * i - c * h), b * f - c * e,
				-(d * i - f * g), a * i - c * g, -(a * f - c * d),
				d * h - e * g, -(a * h - b * g), a * e - b * d,
				invmat);
	return invmat * (1.0f / det);
}

NiMatrix3CLib ToRotationMatrix(NiPoint3CLib p) {
	NiPoint3CLib fwd = NiPoint3CLib(1, 0, 0);
	NiPoint3CLib right = NiPoint3CLib(0, 1, 0);
	NiMatrix3CLib ret;
	ret.entry[0].pt[0] = fwd.x;
	ret.entry[0].pt[1] = right.x;
	ret.entry[0].pt[2] = p.x;
	ret.entry[1].pt[0] = fwd.y;
	ret.entry[1].pt[1] = right.y;
	ret.entry[1].pt[2] = p.y;
	ret.entry[2].pt[0] = fwd.z;
	ret.entry[2].pt[1] = right.z;
	ret.entry[2].pt[2] = p.z;
	return ret;
}

NiPoint3CLib ToDirectionVector(NiMatrix3CLib mat) {
	return NiPoint3CLib(mat.entry[2].pt[0], mat.entry[2].pt[1], mat.entry[2].pt[2]);
}

NiPoint3CLib ToUpVector(NiMatrix3CLib mat) {
	return NiPoint3CLib(mat.entry[1].pt[0], mat.entry[1].pt[1], mat.entry[1].pt[2]);
}

//(Rotation Matrix)^-1 * (World pos - Local Origin)
NiPoint3CLib WorldToLocal(NiPoint3CLib wpos, NiPoint3CLib lorigin, NiMatrix3CLib rot) {
	NiPoint3CLib lpos = wpos - lorigin;
	NiMatrix3CLib invrot = Inverse(rot);
	return invrot * lpos;
}

NiPoint3CLib LocalToWorld(NiPoint3CLib lpos, NiPoint3CLib lorigin, NiMatrix3CLib rot) {
	return rot * lpos + lorigin;
}

NiPoint3CLib CrossProduct(NiPoint3CLib a, NiPoint3CLib b) {
	NiPoint3CLib ret;
	ret[0] = a[1] * b[2] - a[2] * b[1];
	ret[1] = a[2] * b[0] - a[0] * b[2];
	ret[2] = a[0] * b[1] - a[1] * b[0];
	return ret;
}

float DotProduct(NiPoint3CLib a, NiPoint3CLib b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Length(NiPoint3CLib p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

NiPoint3CLib Normalize(NiPoint3CLib p) {
	NiPoint3CLib ret = p;
	float l = Length(p);
	if (l == 0) {
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
	}
	ret.x /= l;
	ret.y /= l;
	ret.z /= l;
	return ret;
}
