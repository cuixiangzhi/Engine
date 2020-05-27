#include "Mesh.h"
#include "../Math/AABB.h"
#include "../Math/Matrix4x4.h"

namespace Engine
{
	Fragment Fragment::Lerp(const Fragment& from, const Fragment& to, float t,bool plerp)
	{
		Fragment result;
		//屏幕坐标XY是线性插值,Z存储的是1/Z也是线性插值
		result.position = Vector4::Lerp(from.position, to.position, t);
		//顶点属性透视校正插值 datat=lerp(data0/z0,data1/z1,t)/lerp(1/z0,1/z1,t)
		float fromz = from.position.w, toz = to.position.w, tz = 1.0f / result.position.w;
		if (!plerp) { fromz = 1.0f; toz = 1.0f; tz = 1.0f; }
		result.color = Color::Lerp(from.color * fromz, to.color * toz, t) * tz;
		result.normal = Vector3::Lerp(from.normal * fromz, to.normal * toz, t) * tz;
		result.texcoord = Vector2::Lerp(from.texcoord * fromz, to.texcoord * toz, t) * tz;
		result.viewRay = Vector3::Lerp(from.viewRay * fromz, to.viewRay * toz, t) * tz;
		result.worldPos = Vector3::Lerp(from.worldPos * fromz, to.worldPos * toz, t) * tz;
		return result;
	}

	bool Mesh::LoadFromFile(const char* fileName)
	{
		FILE* fp = NULL;
		if (fopen_s(&fp, fileName, "rb") != 0) return false;
		//读取顶点数和面数
		if (fscanf_s(fp, "%d %d", &vertexCount, &faceCount) != 2) goto LOAD_FAILED;
		//分配内存
		auto vertexDataSize = sizeof(Vertex)* vertexCount;
		auto faceDataSzie = sizeof(Face)* faceCount;
		char* bufferPTR = (char*)malloc(vertexDataSize + faceDataSzie);
		vertexData = (Vertex*)bufferPTR;
		faceData = (Face*)(bufferPTR + vertexDataSize);
		//读文件初始化
		int vertexIndex = 0, faceIndex = 0, readcount = 0; char datatype;
		float px, py, pz, nx, ny, nz, cx, cy, cz, ux, uy;
		while (fscanf_s(fp, "%c", &datatype) == 1)
		{
			if (datatype == 'v')
			{
				readcount = fscanf_s(fp, "%f %f %f %f %f %f %f %f %f %f %f", &px, &py, &pz, &nx, &ny, &nz, &cx, &cy, &cz, &ux, &uy);
				if (readcount != 11) goto LOAD_FAILED;
				Vertex& v = vertexData[vertexIndex++];
				v.position.Set(px, py, pz);
				v.normal.Set(nx, ny, nz);
				v.color.Set(cx, cy, cz);
				v.uv.Set(ux, uy);
			}
			else if (datatype == 'f')
			{
				Face& f = faceData[faceIndex++];
				readcount = fscanf_s(fp, "%d %d %d", &f.v0, &f.v1, &f.v2);
				if (readcount != 3) goto LOAD_FAILED;
			}
		}
		if (vertexIndex != vertexCount || faceIndex != faceCount) goto LOAD_FAILED;
		fclose(fp);
		return true;
	LOAD_FAILED:
		if (fp) fclose(fp);
		return false;
	}

	AABB Mesh::CalculateAABB(const Matrix4x4& localToWorld) const
	{
		AABB aabb;
		for (int i = 0; i < vertexCount; ++i)
		{
			Vertex& v = vertexData[i];
			Vector3 wp = localToWorld.MultiplyPoint(v.position);
			aabb.Encapsulate(wp);
		}
		return aabb;
	}
}