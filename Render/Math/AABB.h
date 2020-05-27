#pragma once
#include "Vector3.h"

namespace Engine
{
	class AABB
	{
	public:
		Vector3 minp;
		Vector3 maxp;
	public:
		AABB() : minp({ FLT_MAX, FLT_MAX, FLT_MAX }), maxp({ FLT_MIN, FLT_MIN, FLT_MIN }) {}
		AABB(const Vector3& inMin, const Vector3& inMax) : minp(inMin), maxp(inMax) {}
		void Set(const Vector3& inMin, const Vector3& inMax) { minp = inMin; maxp = inMax; }
	public:
		const Vector3& GetMin() const { return minp; }
		const Vector3& GetMax() const { return maxp; }
		Vector3 GetCenter() const { return (maxp + minp) * 0.5f; }
		Vector3 GetExtent() const { return (maxp - minp) * 0.5f; }
		void Encapsulate(const AABB& aabb) { minp = Vector3::Min(minp, aabb.GetMin()); maxp = Vector3::Max(maxp, aabb.GetMax()); }
		void Encapsulate(const Vector3& position) { minp = Vector3::Min(minp, position); maxp = Vector3::Max(maxp, position); }
	};
}