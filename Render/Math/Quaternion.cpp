#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4x4.h"

namespace Engine
{
	Quaternion Quaternion::operator* (const Quaternion& q)
	{
		//复数乘法化简
		float qx = x * q.w + w * q.x + y * q.z - z * q.y;
		float qy = y * q.w + w * q.y + z * q.x - x * q.z;
		float qz = z * q.w + w * q.z + x * q.y - y * q.x;
		float qw = w * q.w - x * q.x - y * q.y - z * q.z;
		return Quaternion(qx, qy, qz, qw);
	}

	Vector3 Quaternion::operator*(const Vector3& v)
	{
		//四元数转矩阵乘以向量
		Matrix4x4 matrix;
		ToMatrix4x4(*this, matrix);
		return matrix.MultiplyVector(v);
	}

	Quaternion Quaternion::Inverse(const Quaternion& q)
	{
		//四元数的逆等于共轭
		return Quaternion(-q.x, -q.y, -q.z, q.w);
	}

	Quaternion Quaternion::Euler(const Vector3& eulerAngles)
	{
		//旋转顺序z-x-y
		float halfx = Math::Deg2Rad(eulerAngles.x * 0.5f);
		float halfy = Math::Deg2Rad(eulerAngles.y * 0.5f);
		float halfz = Math::Deg2Rad(eulerAngles.z * 0.5f);

		//XYZ轴旋转四元数
		Quaternion qx = Quaternion(sin(halfx), 0, 0, cos(halfx));
		Quaternion qy = Quaternion(0, sin(halfy), 0, cos(halfy));
		Quaternion qz = Quaternion(0, 0, sin(halfz), cos(halfz));

		return qy * qx * qz;
	}

	void Quaternion::ToMatrix4x4(const Quaternion& rotation, Matrix4x4& matrix)
	{
		const Quaternion& q = rotation;

		float xx = 2 * q.x * q.x;
		float yy = 2 * q.y * q.y;
		float zz = 2 * q.z * q.z;
		float xy = 2 * q.x * q.y;
		float wz = 2 * q.w * q.z;
		float xz = 2 * q.x * q.z;
		float wy = 2 * q.w * q.y;
		float yz = 2 * q.y * q.z;
		float wx = 2 * q.w * q.x;

		matrix.m00 = 1 - yy - zz;
		matrix.m01 = xy - wz;
		matrix.m02 = xz + wy;
		matrix.m03 = 0;

		matrix.m10 = xy + wz;
		matrix.m11 = 1 - xx - zz;
		matrix.m12 = yz - wx;
		matrix.m13 = 0;

		matrix.m20 = xz - wy;
		matrix.m21 = yz + wx;
		matrix.m22 = 1 - xx - yy;
		matrix.m23 = 0;

		matrix.m30 = 0;
		matrix.m31 = 0;
		matrix.m32 = 0;
		matrix.m33 = 1;
	}
}