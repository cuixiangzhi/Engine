#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace Engine
{
	Matrix4x4 Matrix4x4::identity = Matrix4x4();

	Matrix4x4::Matrix4x4()
	{
		m00 = 1, m01 = 0, m02 = 0, m03 = 0;
		m10 = 0, m11 = 1, m12 = 0, m13 = 0;
		m20 = 0, m21 = 0, m22 = 1, m23 = 0;
		m30 = 0, m31 = 0, m32 = 0, m33 = 1;
	}

	void Matrix4x4::SetOrtho(float orthoSize, float aspect, float nearPlane, float farPlane)
	{
		float halfHeight = orthoSize;
		float halfWidth = orthoSize * aspect;

		float x = 1.0f / halfWidth;
		float y = 1.0f / halfHeight;
		float a = 2.0f / (farPlane - nearPlane);
		float b = (nearPlane + farPlane) / (nearPlane - farPlane);

		m00 = x; m01 = 0; m02 = 0; m03 = 0;
		m10 = 0; m11 = y; m12 = 0; m13 = 0;
		m20 = 0; m21 = 0; m22 = a; m23 = b;
		m30 = 0; m31 = 0; m32 = 0; m33 = 1;
	}

	//透视投影
	void Matrix4x4::SetPerspective(float hfov, float aspect, float nearPlane, float farPlane)
	{
		float halfHeight = nearPlane * tan(Math::Deg2Rad(hfov) * 0.5f);
		float halfWidth = halfHeight * aspect;
		float x = nearPlane / halfWidth;
		float y = nearPlane / halfHeight;
		float a = (farPlane + nearPlane) / (farPlane - nearPlane);
		float b = 2 * nearPlane * farPlane / (nearPlane - farPlane);

		m00 = x; m01 = 0; m02 = 0; m03 = 0;
		m10 = 0; m11 = y; m12 = 0; m13 = 0;
		m20 = 0; m21 = 0; m22 = a; m23 = b;
		m30 = 0; m31 = 0; m32 = 1; m33 = 0;
	}

	//模型矩阵
	void Matrix4x4::SetTRS(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale)
	{
		//旋转顺序z-x-y
		auto rotation = Quaternion::Euler(eulerAngles);
		Quaternion::ToMatrix4x4(rotation, *this);

		//旋转*缩放
		m00 *= scale.x; m01 *= scale.y; m02 *= scale.z;
		m10 *= scale.x; m11 *= scale.y; m12 *= scale.z;
		m20 *= scale.x; m21 *= scale.y; m22 *= scale.z;

		//平移*RS
		m03 = position.x; m13 = position.y; m23 = position.z;
	}

	//观察矩阵
	void Matrix4x4::SetTRSInverse(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale)
	{
		//旋转逆变换
		auto rotation = Quaternion::Inverse(Quaternion::Euler(eulerAngles));
		Quaternion::ToMatrix4x4(rotation, *this);

		//平移逆变换 R逆*T逆
		float npx = -position.x, npy = -position.y, npz = -position.z;
		m03 = m00 * npx + m01 * npy + m02 * npz;
		m13 = m10 * npx + m11 * npy + m12 * npz;
		m23 = m20 * npx + m21 * npy + m22 * npz;

		//缩放逆变换 S逆*R逆*T逆
		float nsx = 1.0f / scale.x, nsy = 1.0f / scale.y, nsz = 1.0f / scale.z;
		m00 *= nsx; m01 *= nsx; m02 *= nsx; m03 *= nsx;
		m10 *= nsy; m11 *= nsy; m12 *= nsy; m13 *= nsy;
		m20 *= nsz; m21 *= nsz; m22 *= nsz; m23 *= nsz;
	}

	//矩阵乘法
	Matrix4x4 Matrix4x4::operator* (const Matrix4x4& rhs) const
	{
		Matrix4x4 result;

		result.m00 = m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30;
		result.m01 = m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31;
		result.m02 = m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32;
		result.m03 = m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33;

		result.m10 = m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30;
		result.m11 = m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
		result.m12 = m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
		result.m13 = m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;

		result.m20 = m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30;
		result.m21 = m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
		result.m22 = m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
		result.m23 = m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;

		result.m30 = m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30;
		result.m31 = m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
		result.m32 = m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
		result.m33 = m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;

		return result;
	}

	//齐次坐标
	Vector4 Matrix4x4::operator*(const Vector4& p) const
	{
		float x = m00 * p.x + m01*p.y + m02*p.z + m03*p.w;
		float y = m10 * p.x + m11*p.y + m12*p.z + m13*p.w;
		float z = m20 * p.x + m21*p.y + m22*p.z + m23*p.w;
		float w = m30 * p.x + m31*p.y + m32*p.z + m33*p.w;
		return Vector4(x, y, z, w);
	}

	//坐标
	Vector3 Matrix4x4::MultiplyPoint(const Vector3& p) const
	{
		float x = m00 * p.x + m01*p.y + m02*p.z + m03;
		float y = m10 * p.x + m11*p.y + m12*p.z + m13;
		float z = m20 * p.x + m21*p.y + m22*p.z + m23;
		float w = m30 * p.x + m31*p.y + m32*p.z + m33;
		return Vector3(x / w, y / w, z / w);
	}

	//向量
	Vector3 Matrix4x4::MultiplyVector(const Vector3& v) const
	{
		float x = m00 * v.x + m01*v.y + m02*v.z;
		float y = m10 * v.x + m11*v.y + m12*v.z;
		float z = m20 * v.x + m21*v.y + m22*v.z;
		return Vector3(x, y, z);
	}
}