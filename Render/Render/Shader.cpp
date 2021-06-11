#include "Shader.h"
#include "../Device/Device.h"
#include "../Render/Mesh.h"
#include "../Render/Object.h"
#include "../Render/Light.h"
#include "../Render/Camera.h"
#include "../Render/Texture.h"
#include "../Math/Color.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"
#include "../Math/Transform.h"
#include "../Logic.h"

namespace Engine
{
	Shader::Shader()
	{
		mBlendMode = ShaderBlend::BLEND_OFF;
		mZWriteMode = ShaderZWrite::ZWRITE_ON;
	}

	void Shader::SetDevice(Device* device)
	{
		mDevice = device;
	}

	void Shader::SetObject(Object* object)
	{
		mObject = object;
	}

	void Shader::SetLight(ShaderProperty propertyKey, Light* light)
	{
		mLightMap[propertyKey] = light;
	}

	void Shader::SetCamera(ShaderProperty propertyKey, Camera* camera)
	{
		mCameraMap[propertyKey] = camera;
	}

	void Shader::SetTexture(ShaderProperty propertyKey, Texture* texture)
	{
		mTextureMap[propertyKey] = texture;
	}

	Color Shader::tex2D(Texture* texture, const Vector2& uv)
	{
		Texture2D* texture2D = dynamic_cast<Texture2D*>(texture);
		//REPEAT模式,点采样
		float x = uv.x * (texture2D->GetWidth() - 1);
		float y = uv.y * (texture2D->GetHeight() - 1);
		int roundx = (int)Math::Roundf(x);
		int roundy = (int)Math::Roundf(y);
		return texture2D->GetPixel(roundx, roundy);
	}

	Color Shader::texCUBE(Texture* texture, const Vector3& viewDir)
	{
		Cubemap* cubemap = dynamic_cast<Cubemap*>(texture);
		float absx = abs(viewDir.x), absy = abs(viewDir.y), absz = abs(viewDir.z);
		int face = 0; float maxv = max(max(absx, absy), absz); float u, v;
		//根据方向的最大维决定采样面和UV坐标
		if (maxv == absx) { face = viewDir.x > 0.0f ? 0 : 1; u = viewDir.z; v = viewDir.y; }
		else if (maxv == absy) { face = viewDir.y > 0.0f ? 2 : 3; u = viewDir.x; v = viewDir.z; }
		else if (maxv == absz) { face = viewDir.z > 0.0f ? 4 : 5; u = viewDir.x; v = viewDir.y; }
		if (face == 0 || face == 5) { u = -u; }
		else if (face == 2) { v = -v; }
		//uv映射到[0,1]
		u = Math::Clamp(u / maxv * 0.5f + 0.5f, 0.0f, 1.0f);
		v = Math::Clamp(v / maxv * 0.5f + 0.5f, 0.0f, 1.0f);
		//点采样
		float x = u * (cubemap->GetWidth() - 1);
		float y = v * (cubemap->GetHeight() - 1);
		int roundx = (int)Math::Roundf(x);
		int roundy = (int)Math::Roundf(y);

		return cubemap->GetPixel(face, roundx, roundy);
	}

	float Shader::getShadow(Camera* shadowCamera, Texture* shadowTexture, const Vector3& worldPos)
	{
		Vector3 shadowCoord = vpMatrix(shadowCamera).MultiplyPoint(worldPos);
		shadowCoord *= 0.5f; shadowCoord += 0.5f;
		RenderTexture* renderTexture = dynamic_cast<RenderTexture*>(shadowTexture);
		//
		float x = shadowCoord.x * (renderTexture->GetWidth() - 1);
		float y = shadowCoord.y * (renderTexture->GetHeight() - 1);
		int roundx = (int)Math::Roundf(x);
		int roundy = (int)Math::Roundf(y);
		float shadowDepth = renderTexture->GetDepth(roundx, roundy);
		float pixelDepth = Math::Clamp(shadowCoord.z, 0.0f, 1.0f)-0.01f;

		return pixelDepth < shadowDepth ? 1.0f : 0.0f;
	}

	Matrix4x4 Shader::mMatrix(Object* object)
	{
		return object->transform->GetLocalToWorldMatrix();
	}

	Matrix4x4 Shader::vpMatrix(Camera* camera)
	{
		return camera->GetProjectionMatrix() * camera->GetViewMatrix();
	}

	Matrix4x4 Shader::mvpMatrix(Camera* camera, Object* object)
	{
		return camera->GetProjectionMatrix() * camera->GetViewMatrix() * object->transform->GetLocalToWorldMatrix();
	}

	void Shader::cvvcull(vector<Fragment>& input, vector<Fragment>& output, int clipside)
	{
		output.clear(); int v0code, v1code; float v0w, v1w, vf, vt, top, bot; Fragment vpf;
		int size = (int)input.size();
		for (int i = 0, j = size - 1; i < size; ++i, j = i - 1)
		{
			Fragment& v0f = input[j]; Vector4& v0p = v0f.position;
			Fragment& v1f = input[i]; Vector4& v1p = v1f.position;

			//端点编码和起始点坐标
			v0w = v0p.w + Math::EPSILON; v1w = v1p.w + Math::EPSILON;
			if (clipside == 0) { v0code = v0p.x >= -v0w ? 0 : 1; v1code = v1p.x >= -v1w ? 0 : 1; vf = v0p.x; vt = v1p.x; }
			else if (clipside == 1) { v0code = v0p.x <= v0w ? 0 : 1; v1code = v1p.x <= v1w ? 0 : 1; vf = v0p.x; vt = v1p.x; }
			else if (clipside == 2) { v0code = v0p.y >= -v0w ? 0 : 1; v1code = v1p.y >= -v1w ? 0 : 1; vf = v0p.y; vt = v1p.y; }
			else if (clipside == 3) { v0code = v0p.y <= v0w ? 0 : 1; v1code = v1p.y <= v1w ? 0 : 1; vf = v0p.y; vt = v1p.y; }
			else if (clipside == 4) { v0code = v0p.z >= -v0w ? 0 : 1; v1code = v1p.z >= -v1w ? 0 : 1; vf = v0p.z; vt = v1p.z; }
			else if (clipside == 5) { v0code = v0p.z <= v0w ? 0 : 1; v1code = v1p.z <= v1w ? 0 : 1; vf = v0p.z; vt = v1p.z; }

			//计算交点(裁剪面平移至原点可以统一坐标系)
			if (v0code != v1code)
			{
				if (clipside % 2 == 0) { top = (vf + v0p.w); bot = (vf + v0p.w) - (vt + v1p.w); }
				else { top = (vf - v0p.w); bot = (vf - v0p.w) - (vt - v1p.w); }
				if (abs(bot) <= Math::EPSILON) vpf = v1f;
				else vpf = Fragment::Lerp(v0f, v1f, top / bot, false);
			}

			//都在外侧,不输出
			if (v0code == v1code && v0code != 0) continue;
			//都在内侧,输出终点
			else if (v0code == v1code && v0code == 0) output.push_back(v1f);
			//起点在内终点在外,输出交点
			else if (v0code != v1code && v0code == 0) output.push_back(vpf);
			//起点在外终点在内,输出交点和终点
			else if (v0code != v1code && v1code == 0) { output.push_back(vpf); output.push_back(v1f); }
		}
		if (++clipside <= 5) cvvcull(output, input, clipside);
	}

	void Shader::division(Fragment& v0, Fragment& v1, Fragment& v2)
	{
		Vector4& v0p = v0.position; float v0w = 1.0f / v0p.w;
		v0p.x *= v0w; v0p.y *= v0w; v0p.z *= v0w; v0p.w = v0w;

		Vector4& v1p = v1.position; float v1w = 1.0f / v1p.w;
		v1p.x *= v1w; v1p.y *= v1w; v1p.z *= v1w; v1p.w = v1w;

		Vector4& v2p = v2.position; float v2w = 1.0f / v2p.w;
		v2p.x *= v2w; v2p.y *= v2w; v2p.z *= v2w; v2p.w = v2w;
	}

	bool Shader::backcull(const Fragment& v0, const Fragment& v1, const Fragment& v2)
	{
		if (Logic::GetRenderMode() == RenderMode::RENDER_WIREFRAME) return true;
		//NDC空间下只需要判断法线Z值即可,视线方向始终是(0,0,1)
		Vector4 v0v1 = v1.position - v0.position;
		Vector4 v0v2 = v2.position - v0.position;
		float nz = v0v1.x * v0v2.y - v0v1.y * v0v2.x;
		return nz < 0.0f;
	}

	void Shader::viewport(Fragment& v0, Fragment& v1, Fragment& v2)
	{
		int deviceWidth = mDevice->GetWidth();
		int deviceHeight = mDevice->GetHeight();

		Vector4& v0p = v0.position;
		v0p.x = (v0p.x * 0.5f + 0.5f) * deviceWidth;
		v0p.y = (v0p.y * 0.5f + 0.5f) * deviceHeight;
		v0p.z = (v0p.z * 0.5f + 0.5f);

		Vector4& v1p = v1.position;
		v1p.x = (v1p.x * 0.5f + 0.5f) * deviceWidth;
		v1p.y = (v1p.y * 0.5f + 0.5f) * deviceHeight;
		v1p.z = (v1p.z * 0.5f + 0.5f);

		Vector4& v2p = v2.position;
		v2p.x = (v2p.x * 0.5f + 0.5f) * deviceWidth;
		v2p.y = (v2p.y * 0.5f + 0.5f) * deviceHeight;
		v2p.z = (v2p.z * 0.5f + 0.5f);
	}

	bool Shader::depthtest(float srcdepth, float dstdepth)
	{
		return dstdepth - srcdepth > 0.0f;
	}

	void Shader::updatecolor(RenderTexture* renderTarget, int x, int y, Color& color)
	{
		switch (mBlendMode)
		{
		case BLEND_OFF:
			renderTarget->SetPixel(x, y, color);
			break;
		case BLEND_ALPHA:
			Color finalcolor = Color::Lerp(renderTarget->GetPixel(x, y), color, color.a);
			finalcolor.a = color.a;
			renderTarget->SetPixel(x, y, finalcolor);
			break;
		}
	}

	void Shader::updatedepth(RenderTexture* renderTarget, int x, int y, float depth)
	{
		switch (mZWriteMode)
		{
		case ZWRITE_ON:
			renderTarget->SetDepth(x, y, depth);
			break;
		case ZWRITE_OFF:
			break;
		}
	}

	Fragment Shader::vertex(const Vertex& v)
	{
		Fragment o;
		Camera* camera = mCameraMap[ShaderProperty::CAMERA_MAIN];
		o.position = mvpMatrix(camera, mObject) * Vector4(v.position, 1.0f);
		o.normal = v.normal;
		o.texcoord = v.uv;
		o.color = v.color;
		return o;
	}

	Color Shader::fragment(const Fragment& v)
	{
		return v.color;
	}

	ShadowShader::ShadowShader() : Shader()
	{
		mBlendMode = ShaderBlend::BLEND_OFF;
		mZWriteMode = ShaderZWrite::ZWRITE_ON;
	}

	Fragment ShadowShader::vertex(const Vertex& v)
	{
		Fragment o = Shader::vertex(v);
		o.position.z += 0.01f;
		return o;
	}

	SkyboxShader::SkyboxShader() :Shader()
	{
		mBlendMode = ShaderBlend::BLEND_OFF;
		mZWriteMode = ShaderZWrite::ZWRITE_OFF;
	}

	Fragment SkyboxShader::vertex(const Vertex& v)
	{
		Fragment o;
		Vector3 localPosition;
		Camera* camera = mCameraMap[ShaderProperty::CAMERA_MAIN];
		float cameraFOV = camera->mFieldOfView;
		float cameraFarPlane = camera->mFarClipPlane;
		float farHalfHeight = cameraFarPlane * tan(Math::Deg2Rad(cameraFOV * 0.5f));
		float farHalfWidth = farHalfHeight * camera->mAspect;
		if (v.uv.x == 0.0f && v.uv.y == 0.0f) localPosition = Vector3(-farHalfWidth, -farHalfHeight, cameraFarPlane);
		else if (v.uv.x == 0.0f && v.uv.y == 1.0f) localPosition = Vector3(-farHalfWidth, farHalfHeight, cameraFarPlane);
		else if (v.uv.x == 1.0f && v.uv.y == 0.0f) localPosition = Vector3(farHalfWidth, -farHalfHeight, cameraFarPlane);
		else if (v.uv.x == 1.0f && v.uv.y == 1.0f) localPosition = Vector3(farHalfWidth, farHalfHeight, cameraFarPlane);
		
		o.position = Vector4(v.position, 1.0f + Math::EPSILON);
		o.viewRay = camera->transform->GetRotation() * localPosition;
		return o;
	}

	Color SkyboxShader::fragment(const Fragment& v)
	{
		Cubemap* cubemap = dynamic_cast<Cubemap*>(mTextureMap[ShaderProperty::TEXTURE_MAIN]);
		return texCUBE(cubemap, v.viewRay);
	}

	NormalShader::NormalShader() :Shader()
	{
		mBlendMode = BLEND_ALPHA;
		mZWriteMode = ZWRITE_ON;
	}

	Shader* NormalShader::Create(ShaderBlend blendMode, ShaderZWrite zwriteMode)
	{
		NormalShader* shader = new NormalShader();
		shader->mBlendMode = blendMode;
		shader->mZWriteMode = zwriteMode;
		return shader;
	}

	Fragment NormalShader::vertex(const Vertex& v)
	{
		Fragment o = Shader::vertex(v);
		o.worldPos = mMatrix(mObject).MultiplyPoint(v.position);
		return o;
	}

	Color NormalShader::fragment(const Fragment& v)
	{
		//线框模式,返回顶点色插值
		if (Logic::GetRenderMode() == RenderMode::RENDER_WIREFRAME) return v.color;
		Color color = tex2D(mTextureMap[ShaderProperty::TEXTURE_MAIN], v.texcoord);
		//透明物体,返回纹理颜色
		if (mBlendMode == ShaderBlend::BLEND_ALPHA) return color;
		//计算环境光
		Light* light = mLightMap[ShaderProperty::LIGHT_MAIN];
		Color ambient = color * light->GetAmbientColor();
		if (Logic::GetLightState())
		{
			//计算阴影
			float shadowValue = 1.0f;
			if (Logic::GetShadowState())
			{
				Camera* shadowCamera = mCameraMap[ShaderProperty::CAMERA_LIGHT];
				Texture* shadowTexture = (Texture*)mTextureMap[ShaderProperty::TEXTURE_SHADOW];
				shadowValue = getShadow(shadowCamera, shadowTexture, v.worldPos);
			}
			//计算漫反射光
			Vector3 lightForward = light->transform->GetBackward();
			Vector3 pixelNormal = Vector3::Normalize(v.normal);
			float diffuseTerm = max(0.0f, Vector3::Dot(pixelNormal, lightForward));
			Color diffuse = color * light->GetDirectColor() * diffuseTerm * shadowValue;
			return ambient + diffuse;
		}
		else
		{
			return ambient;
		}
	}
}