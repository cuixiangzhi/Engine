#pragma once

namespace Engine
{
	class Vector3;
	class Matrix4x4;
	class Quaternion
	{
	public:
		float x, y, z, w;
	public:
		Quaternion(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW){}
	public:
		Quaternion operator*(const Quaternion& q);
		Vector3 operator*(const Vector3& v);
	public:
		static Quaternion Euler(const Vector3& eulerAngles);
		static Quaternion Inverse(const Quaternion& rotation);
		static void ToMatrix4x4(const Quaternion& rotation, Matrix4x4& matrix);
	};
}