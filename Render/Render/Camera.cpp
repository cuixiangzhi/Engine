#include "Camera.h"
#include "../Render/Object.h"
#include "../Render/Light.h"
#include "../Render/Shader.h"
#include "../Render/Mesh.h"
#include "../Device/Device.h"
#include "../Math/Transform.h"
#include "../Math/AABB.h"
#include "../Logic.h"

namespace Engine
{
	Camera* Camera::Create(bool ortho, float aspect, float fovOrOthoSize, float nearPlane, float farPlane)
	{
		Camera* camera = new Camera();
		camera->transform = new Transform();

		camera->mAspect = aspect;
		camera->mOrthoFlag = ortho;
		camera->mFieldOfView = fovOrOthoSize;
		camera->mNearClipPlane = nearPlane;
		camera->mFarClipPlane = farPlane;
		camera->mOrthoSize = fovOrOthoSize;	

		return camera;
	}

	const Matrix4x4& Camera::GetViewMatrix()
	{
		mViewMatrix.SetTRSInverse(transform->GetPosition(), transform->GetEulerAngles(), Vector3::one);
		return mViewMatrix;
	}

	const Matrix4x4& Camera::GetProjectionMatrix()
	{
		if (mOrthoFlag) mProjectionMatrix.SetOrtho(mOrthoSize, mAspect, mNearClipPlane, mFarClipPlane);
		else mProjectionMatrix.SetPerspective(mFieldOfView, mAspect, mNearClipPlane, mFarClipPlane);
		return mProjectionMatrix;
	}

	void Camera::UpdateShadowView(CullData* cullData)
	{
		//计算AABB
		AABB aabb;
		Matrix4x4 worldToLight = cullData->shadowLight->transform->GetWorldToLocalMatrix();
		for (auto iter = cullData->shadowCasters.begin(); iter != cullData->shadowCasters.end(); ++iter)
		{
			Object* object = *iter;
			Mesh* mesh = object->mesh;
			Matrix4x4 localToLight = worldToLight * object->transform->GetLocalToWorldMatrix();
			aabb.Encapsulate(mesh->CalculateAABB(localToLight));
		}
		//修正光源摄像机
		transform->SetPosition(cullData->shadowLight->transform->GetLocalToWorldMatrix().MultiplyPoint(aabb.GetCenter()));
		transform->SetEulerAngles(cullData->shadowLight->transform->GetEulerAngles());
		Vector3 aabbExtent = aabb.GetExtent();
		mNearClipPlane = -aabbExtent.z - 1.0f;
		mFarClipPlane = aabbExtent.z + 1.0f;
		mOrthoSize = aabbExtent.y;
		mAspect = aabbExtent.x / aabbExtent.y;
	}

	void Camera::Render(CullData* cullData)
	{
		Device* device = Device::GetDevice();

		//渲染主光阴影图
		if (Logic::GetShadowState())
		{
			auto shadowCamera = cullData->shadowCamera;
			shadowCamera->UpdateShadowView(cullData);
			auto shadowShader = cullData->shadowShader;
			shadowShader->SetCamera(ShaderProperty::CAMERA_MAIN, cullData->shadowCamera);
			shadowShader->SetDevice(device);
			device->Clear(cullData->shadowTexture);
			for (auto iter = cullData->shadowCasters.begin(); iter != cullData->shadowCasters.end(); ++iter)
			{
				shadowShader->SetObject(*iter);
				device->DrawObject(*iter, shadowShader);
			}
		}
		
		//渲染不透明物体
		device->Clear(NULL);
		for (auto iter = cullData->opaqueObjects.begin(); iter != cullData->opaqueObjects.end(); ++iter)
		{
			auto object = *iter;
			auto shader = object->shader;
			shader->SetDevice(device);
			shader->SetObject(object);
			shader->SetCamera(ShaderProperty::CAMERA_MAIN, this);
			shader->SetCamera(ShaderProperty::CAMERA_LIGHT, cullData->shadowCamera);
			shader->SetLight(ShaderProperty::LIGHT_MAIN, cullData->shadowLight);
			shader->SetTexture(ShaderProperty::TEXTURE_MAIN, object->texture);
			shader->SetTexture(ShaderProperty::TEXTURE_SHADOW, (Texture*)cullData->shadowTexture);
			device->DrawObject(object, object->shader);
		}

		//渲染天空盒
		if (Logic::GetSkyboxState() && Logic::GetRenderMode() != RenderMode::RENDER_WIREFRAME)
		{
			auto skyboxObject = cullData->skyboxObject;
			auto skyboxShader = skyboxObject->shader;
			skyboxShader->SetDevice(device);
			skyboxShader->SetObject(skyboxObject);
			skyboxShader->SetCamera(ShaderProperty::CAMERA_MAIN, this);
			skyboxShader->SetTexture(ShaderProperty::TEXTURE_MAIN, skyboxObject->texture);
			device->DrawObject(skyboxObject, skyboxShader);
		}

		//渲染透明物体
		for (auto iter = cullData->transparentObjects.begin(); iter != cullData->transparentObjects.end(); ++iter)
		{
			auto object = *iter;
			auto shader = object->shader;
			shader->SetDevice(device);
			shader->SetObject(object);
			shader->SetCamera(ShaderProperty::CAMERA_MAIN, this);
			shader->SetCamera(ShaderProperty::CAMERA_LIGHT, cullData->shadowCamera);
			shader->SetLight(ShaderProperty::LIGHT_MAIN, cullData->shadowLight);
			shader->SetTexture(ShaderProperty::TEXTURE_MAIN, object->texture);
			shader->SetTexture(ShaderProperty::TEXTURE_SHADOW, (Texture*)cullData->shadowTexture);
			device->DrawObject(object, object->shader);
		}
	}
}