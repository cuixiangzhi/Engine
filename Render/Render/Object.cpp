#include "Object.h"
#include "../Render/Mesh.h"
#include "../Math/Transform.h"

namespace Engine
{
	Object::Object()
	{
		mesh = NULL;
		shader = NULL;
		texture = NULL;
		transform = NULL;
	}
	Object::~Object()
	{
		if (mesh != NULL)
		{
			delete mesh;
			mesh = NULL;
		}
		if (transform != NULL)
		{
			delete transform;
			transform = NULL;
		}
		shader = NULL;
		texture = NULL;
	}

	Object* Object::Create(const char* fileName, Shader* shader, Texture* texture)
	{
		Object* object = new Object();
		object->mesh = new Mesh();
		object->shader = shader;
		if (!object->shader) goto CREATE_FAILED;
		object->texture = texture;
		object->transform = new Transform();
		if (!object->mesh->LoadFromFile(fileName)) goto CREATE_FAILED;
		return object;
	CREATE_FAILED:
		delete object;
		return NULL;
	}
}