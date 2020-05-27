#pragma once
#include "Vector3.h"

namespace Engine
{
	class Matrix4x4;
	class Quaternion;
	class Transform
	{
	private:
		Vector3 mLocalPosition;
		Vector3 mLocalScale;
		Vector3 mLocalEulerAngles;

		Transform* mParent;
	private:
		Transform(const Transform&);
		Transform& operator=(const Transform&);
	public:
		Transform();
		~Transform();
	public:
		Vector3 GetPosition() const;
		Quaternion GetRotation() const;
		Vector3 GetEulerAngles() const;
		Vector3 GetLocalScale() const;
		Vector3 GetLocalPosition() const;
		Vector3 GetLocalEulerAngles() const;
		Vector3 GetForward() const;
		Vector3 GetBackward() const;
		Vector3 GetRight() const;
		void SetPosition(const Vector3& position);
		void SetEulerAngles(const Vector3& eulerAngles);
		void SetLocalScale(const Vector3& localScale);
		void SetLocalPosition(const Vector3& localPosition);
		void SetLocalEulerAngles(const Vector3& localEulerAngles);
		Matrix4x4 GetLocalToWorldMatrix() const;
		Matrix4x4 GetWorldToLocalMatrix() const;
	};
}