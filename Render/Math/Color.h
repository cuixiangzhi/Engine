#pragma once
#include "Math.h"

namespace Engine
{
	class Color
	{
	public:
		float r, g, b, a;
	public:
		static const Color clear;
		static const Color white;
		static const Color black;
		static const Color red;
		static const Color green;
		static const Color blue;
		static const Color yellow;
		static const Color cyan;
		static const Color grey;
	public:
		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
		Color(uint32 value) { a = (float)(value >> 24 & 0xFF) / 255.0f; r = (float)(value >> 16 & 0xFF) / 255.0f; g = (float)(value >> 8 & 0xFF) / 255.0f; b = (float)(value & 0xFF) / 255.0f; }
		Color(const Color& value) : r(value.r), g(value.g), b(value.b), a(value.a) {}
		Color(float inR, float inG, float inB, float inA = 1.0f) : r(inR), g(inG), b(inB), a(inA) {}
		void Set(float inR, float inG, float inB, float inA = 1.0f) { r = inR; g = inG; b = inB; a = inA; }
	public:
		Color operator* (float scale) const { return Color(r * scale, g * scale, b * scale, a * scale); }
		Color operator* (const Color& value) const { return Color(r * value.r, g * value.g, b * value.b, a * value.a); }
		Color operator+ (const Color& value) const { return Color(r + value.r, g + value.g, b + value.b, a + value.a); }
		Color& operator= (const Color& value) { r = value.r; g = value.g; b = value.b; a = value.a; return *this; }
		operator uint32() const { return (uint8)(Math::Clamp(a, 0.0f, 1.0f) * 255) << 24 | (uint8)(Math::Clamp(r, 0.0f, 1.0f) * 255) << 16 | (uint8)(Math::Clamp(g, 0.0f, 1.0f) * 255) << 8 | (uint8)(Math::Clamp(b, 0.0f, 1.0f) * 255); }
	public:
		static Color Lerp(const Color& from, const Color& to, float t) 
		{ 
			t = Math::Clamp(t, 0.0f, 1.0f); 
			return to * t + from * (1.0f - t); 
		}
	};
}