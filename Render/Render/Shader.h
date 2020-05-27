#pragma once
#include "../Base/BaseDef.h"

namespace Engine
{
	enum ShaderProperty
	{
		TEXTURE_SHADOW = 0,
		TEXTURE_MAIN = 1,
		CAMERA_MAIN = 2,
		CAMERA_LIGHT = 3,
		LIGHT_MAIN = 4,
	};

	enum ShaderBlend
	{
		BLEND_OFF = 0,
		BLEND_ALPHA = 1,
	};

	enum ShaderZWrite
	{
		ZWRITE_ON = 0,
		ZWRITE_OFF = 1,
	};

	class Object;
	class Device;
	class Light;
	class Camera;
	class Texture;
	class RenderTexture;
	class Vertex;
	class Fragment;
	class Color;
	class Vector2;
	class Vector3;
	class Matrix4x4;
	class Shader
	{
	public:
		ShaderBlend mBlendMode;
		ShaderZWrite mZWriteMode;
		Object* mObject;
		Device* mDevice;
		map<ShaderProperty, Light*> mLightMap;
		map<ShaderProperty, Camera*> mCameraMap;
		map<ShaderProperty, Texture*> mTextureMap;
	public:
		Shader();
		virtual ~Shader() {}
	public:
		void SetDevice(Device* device);
		void SetObject(Object* object);
		void SetTexture(ShaderProperty propertyKey, Texture* texture);
		void SetCamera(ShaderProperty propertyKey, Camera* camera);
		void SetLight(ShaderProperty propertyKey, Light* light);
	public:
		Color tex2D(Texture* texture, const Vector2& uv);
		Color texCUBE(Texture* texture, const Vector3& viewDir);
		float getShadow(Camera* shadowCamera, Texture* shadowTexture, const Vector3& worldPos);
		Matrix4x4 mMatrix(Object* object);
		Matrix4x4 vpMatrix(Camera* camera);
		Matrix4x4 mvpMatrix(Camera* camera, Object* object);

		void cvvcull(vector<Fragment>& input, vector<Fragment>& output, int clipside = 0);
		void division(Fragment& v0, Fragment& v1, Fragment& v2);
		bool backcull(const Fragment& v0, const Fragment& v1, const Fragment& v2);
		void viewport(Fragment& v0, Fragment& v1, Fragment& v2);
		bool depthtest(float srcdepth, float dstdepth);
		void updatecolor(RenderTexture* renderTarget, int x, int y, Color& color);
		void updatedepth(RenderTexture* renderTarget, int x, int y, float depth);

		virtual Fragment vertex(const Vertex& v);
		virtual Color fragment(const Fragment& v);
	};

	class ShadowShader : public Shader
	{
	public:
		ShadowShader();
		virtual ~ShadowShader() {}
	public:
		static Shader* Create() { return new ShadowShader(); }
	public:
		virtual Fragment vertex(const Vertex& v);
	};

	class SkyboxShader : public Shader
	{
	public:
		SkyboxShader();
		virtual ~SkyboxShader() {}
	public:
		static Shader* Create() { return new SkyboxShader(); }
	public:
		virtual Fragment vertex(const Vertex& v);
		virtual Color fragment(const Fragment& v);
	};

	class NormalShader : public Shader
	{
	public:
		NormalShader();
		virtual ~NormalShader() {}
	public:
		static Shader* Create(ShaderBlend blendMode, ShaderZWrite zwriteMode);
	public:
		virtual Fragment vertex(const Vertex& v);
		virtual Color fragment(const Fragment& v);
	};
}