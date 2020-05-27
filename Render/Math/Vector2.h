#pragma once
#include "Math.h"

namespace Engine
{
	class Vector2
	{
	public:
		float x, y;
	public:
		Vector2() : x(0.0f), y(0.0f) {}
		Vector2(float inX, float inY) : x(inX), y(inY) {}
		Vector2(const Vector2& v) : x(v.x), y(v.y) {}
		void Set(float inX, float inY) { x = inX; y = inY; }
	public:
		Vector2 operator* (float scale) const { return Vector2(x * scale, y * scale); }
		Vector2 operator/ (float scale) const { return Vector2(x / scale, y / scale); }
		Vector2 operator+ (const Vector2& value) const { return Vector2(x + value.x, y + value.y); }
	public:
		static Vector2 Lerp(const Vector2& from, const Vector2& to, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);
			return to * t + from * (1.0f - t);
		}
	};
}