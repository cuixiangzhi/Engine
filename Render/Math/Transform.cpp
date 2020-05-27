#include "Transform.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

namespace Engine
{
	Transform::Transform()
	{
		mLocalPosition = Vector3::zero;
		mLocalEulerAngles = Vector3::zero;
		mLocalScale = Vector3::one;
	}

	Transform::~Transform()
	{

	}

	Vector3 Transform::GetPosition() const
	{
		return mLocalPosition;
	}

	Quaternion Transform::GetRotation() const
	{
		return Quaternion::Euler(mLocalEulerAngles);
	}

	Vector3 Transform::GetEulerAngles() const
	{
		return mLocalEulerAngles;
	}

	Vector3 Transform::GetLocalScale() const
	{
		return mLocalScale;
	}

	Vector3 Transform::GetLocalPosition() const
	{
		return mLocalPosition;
	}

	Vector3 Transform::GetLocalEulerAngles() const
	{
		return mLocalEulerAngles;
	}

	Vector3 Transform::GetForward() const
	{
		return Quaternion::Euler(mLocalEulerAngles) * Vector3::forward;
	}

	Vector3 Transform::GetBackward() const
	{
		return Quaternion::Euler(mLocalEulerAngles) * Vector3::back;
	}

	Vector3 Transform::GetRight() const
	{
		return Quaternion::Euler(mLocalEulerAngles) * Vector3::right;
	}

	void Transform::SetPosition(const Vector3& position)
	{
		mLocalPosition = position;
	}

	void Transform::SetEulerAngles(const Vector3& eulerAngles)
	{
		mLocalEulerAngles = eulerAngles;
	}

	void Transform::SetLocalScale(const Vector3& localScale)
	{
		mLocalScale = localScale;
	}

	void Transform::SetLocalPosition(const Vector3& localPosition)
	{
		mLocalPosition = localPosition;
	}

	void Transform::SetLocalEulerAngles(const Vector3& localEulerAngles)
	{
		mLocalEulerAngles = localEulerAngles;
	}

	Matrix4x4 Transform::GetLocalToWorldMatrix() const
	{
		Matrix4x4 matrix;
		matrix.SetTRS(mLocalPosition, mLocalEulerAngles, mLocalScale);
		return matrix;
	}

	Matrix4x4 Transform::GetWorldToLocalMatrix() const
	{
		Matrix4x4 matrix;
		matrix.SetTRSInverse(mLocalPosition, mLocalEulerAngles, mLocalScale);
		return matrix;
	}
}