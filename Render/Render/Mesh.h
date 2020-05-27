#pragma once
#include "../Math/Color.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

namespace Engine
{
	class Vertex
	{
	public:
		Vector3 position;
		Vector3 normal;
		Color color;
		Vector2 uv;
	public:
		Vertex(const Vector3& inPosition, const Color& inColor, const Vector3& inNormal, const Vector2& inUV) : position(inPosition), color(inColor), normal(inNormal), uv(inUV) {}
	};

	class Face
	{
	public:
		int v0, v1, v2;
	public:
		Face(int inV0, int inV1, int inV2) : v0(inV0), v1(inV1), v2(inV2) {}
	};

	class Fragment
	{
	public:
		Vector4 position;
		Color color;
		Vector3 normal;
		Vector2 texcoord;
		Vector3 viewRay;
		Vector3 worldPos;
	public:
		static Fragment Lerp(const Fragment& from, const Fragment& to, float t, bool plerp = true);
	};

	class AABB;
	class Matrix4x4;
	class Mesh
	{
	public:
		int vertexCount;
		int faceCount;
		Vertex* vertexData;
		Face* faceData;
	public:
		Mesh() { vertexData = NULL; faceData = NULL; }
		~Mesh() { if (vertexData != NULL) { free(vertexData); vertexData = NULL; faceData = NULL; } }
	private:
		Mesh(const Mesh& mesh);
		Mesh& operator=(const Mesh& mesh);
	public:
		bool LoadFromFile(const char* fileName);
		AABB CalculateAABB(const Matrix4x4& localToWorld) const;
	};
}