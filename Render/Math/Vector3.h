#pragma once
#include "Math.h"

namespace Engine
{
	class Vector3
	{
	public:
		float x, y, z;
	public:
		static const Vector3 one;
		static const Vector3 zero;
		static const Vector3 forward;
		static const Vector3 back;
		static const Vector3 left;
		static const Vector3 right;
		static const Vector3 up;
		static const Vector3 down;
	public:
		Vector3() : x(0.0f), y(0.0f), z(0.0f){}
		Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
		void Set(float inX, float inY, float inZ) { x = inX; y = inY; z = inZ; }
		float Length() const { return sqrt(x * x + y * y + z * z); }
	public:
		Vector3 operator+ (const Vector3& value) const { return Vector3(x + value.x, y + value.y, z + value.z); }
		Vector3 operator- (const Vector3& value) const { return Vector3(x - value.x, y - value.y, z - value.z); }
		Vector3 operator* (const Vector3& value) const { return Vector3(x * value.x, y * value.y, z * value.z); }
		Vector3 operator* (float scale) const { return Vector3(x * scale, y * scale, z * scale); }
		Vector3 operator/ (float scale) const { return Vector3(x / scale, y / scale, z / scale); }
		Vector3& operator*= (float scale) { x *= scale; y *= scale; z *= scale; return *this; }
		Vector3& operator+= (float scale) { x += scale; y += scale; z += scale; return *this; }
	public:
		static Vector3 Normalize(const Vector3& value)
		{
			float length = value.Length();
			if (length > Math::EPSILON)
				return value / length;
			else
				return zero;
		}

		static Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		static float Dot(const Vector3& lhs,const Vector3& rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		static Vector3 Lerp(const Vector3& from, const Vector3& to, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);
			return to * t + from * (1.0f - t);
		}

		static Vector3 Min(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(min(lhs.x, rhs.x), min(lhs.y, rhs.y), min(lhs.z, rhs.z));
		}

		static Vector3 Max(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(max(lhs.x, rhs.x), max(lhs.y, rhs.y), max(lhs.z, rhs.z));
		}

		static bool Equals(const Vector3& lhs, const Vector3& rhs)
		{
			return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
		}
	};
}