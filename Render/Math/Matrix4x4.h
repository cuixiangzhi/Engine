#pragma once

namespace Engine
{
	class Vector3;
	class Vector4;
	class Matrix4x4
	{
	public:
		static Matrix4x4 identity;
	public:
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	public:
		Matrix4x4();
	public:
		void SetOrtho(float hsize, float aspect, float nearPlane, float farPlane);
		void SetPerspective(float hfov, float aspect, float nearPlane, float farPlane);
		void SetTRS(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale);
		void SetTRSInverse(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale);
	public:
		Matrix4x4 operator*(const Matrix4x4& m) const;
		Vector4 operator*(const Vector4& v) const;
	public:
		Vector3 MultiplyPoint(const Vector3& p) const;
		Vector3 MultiplyVector(const Vector3& v) const;
	};
}