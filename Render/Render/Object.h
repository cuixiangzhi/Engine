#pragma once

namespace Engine
{
	class Mesh;
	class Shader;
	class Texture;
	class Transform;
	class Object
	{
	public:
		Mesh* mesh;
		Shader* shader;
		Texture* texture;
		Transform* transform;
	public:
		Object();
		~Object();
	public:
		static Object* Create(const char* fileName, Shader* shader, Texture* texture);
	};
}