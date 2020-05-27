#pragma once
#include "../Base/BaseDef.h"

namespace Engine
{
	class Object;
	class Shader;
	class Window;
	class Vertex;
	class Fragment;
	class RenderTexture;
	class Device
	{
	public:
		static Device* mDeviceInstance;
	public:
		Window* mWindow;
		Shader* mShader;
		RenderTexture* mDefaultTarget;
		RenderTexture* mActiveTarget;
	public:
		Device();
		~Device();
	public:
		static Device* Create(Window* window);
		static Device* GetDevice() { return mDeviceInstance; }
	public:
		void Clear(RenderTexture* renderTarget);
		void Present();
	public:
		int GetWidth() const;
		int GetHeight() const;
	public:
		void DrawObject(Object* mesh, Shader* shader);
	private:
		void DrawPixel(const Fragment& v);
		void DrawLineDDA(Fragment v0, Fragment v1);
		void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v3);
		void DrawTriangleScanline(Fragment v0, Fragment v1, Fragment v2);
	};
}