#pragma once
#include "../Math/Math.h"
#include "../Math/Matrix4x4.h"

namespace Engine
{
	class Light;
	class Camera;
	class Object;
	class Shader;
	class RenderTexture;
	class CullData
	{
	public:
		vector<Object*> shadowCasters;
		vector<Object*> opaqueObjects;
		vector<Object*> transparentObjects;

		Light* shadowLight;
		Camera* shadowCamera;
		Shader* shadowShader;
		RenderTexture* shadowTexture;

		Object* skyboxObject;
	};

	class Transform;
	class Camera
	{
	public:
		float mAspect;
		float mFieldOfView;
		float mNearClipPlane;
		float mFarClipPlane;
		float mOrthoSize;
		bool mOrthoFlag;

		Transform* transform;
		Matrix4x4 mViewMatrix;
		Matrix4x4 mProjectionMatrix;
		static Camera* Create(bool ortho, float aspect, float fovOrOthoSize, float nearPlane, float farPlane);
	public:
		const Matrix4x4& GetViewMatrix();
		const Matrix4x4& GetProjectionMatrix();
		const Matrix4x4& GetClipToWorldMatrix();
	public:
		void Render(CullData* cullData);
		void UpdateShadowView(CullData* cullData);
	};
}