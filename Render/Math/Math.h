#pragma once
#include "../Base/BaseDef.h"

namespace Engine
{
	namespace Math
	{
		static const float EPSILON = 1e-05f;

		static const float PI = 3.1415926f;

		static const float GAMMA = 2.2f;

		static const float INVGAMMA = 0.45f;

		static float Deg2Rad(float degree) { return degree * PI / 180.0f; }

		static float Rad2Deg(float radian) { return radian * 180.0f / PI; }

		static float Clamp(float value, float minv, float maxv) { if (value > maxv) return maxv; if (value < minv) return minv; return value; }

		static float Lerp(float from, float to, float t) { t = Clamp(t, 0.0f, 1.0f); return to * t + from * (1.0f - t); }

		static float Roundf(float value, float deltaValue = 0.01f) { return roundf(value + deltaValue); }

		static float Floorf(float value, float deltaValue = 0.01f) { return floorf(value + deltaValue); }
	}
}