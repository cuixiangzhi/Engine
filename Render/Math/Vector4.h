#pragma once
#include "Vector3.h"

namespace Engine
{
	class Vector4
	{
	public:
		float x, y, z, w;
	public:
		Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Vector4(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
		Vector4(const Vector3& v, float inW) : x(v.x), y(v.y), z(v.z), w(inW) {}
		Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	public:
		Vector4 operator* (float scale) const { return Vector4(x * scale, y * scale, z * scale, w * scale); }
		Vector4 operator/ (float scale) const { return Vector4(x / scale, y / scale, z / scale, w / scale); }
		Vector4 operator+ (const Vector4& value) const { return Vector4(x + value.x, y + value.y, z + value.z, w + value.w); }
		Vector4 operator- (const Vector4& value) const { return Vector4(x - value.x, y - value.y, z - value.z, w - value.w); }
	public:
		static Vector4 Lerp(const Vector4& from, const Vector4& to, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);
			return to * t + from * (1.0f - t);
		}
	};
}