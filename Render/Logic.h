#pragma once
#include "Base/BaseDef.h"

namespace Engine
{
	enum RenderMode
	{
		RENDER_WIREFRAME = 0,
		RENDER_SHADED = 1,
		RENDER_MAX = 2,
	};

	enum ControlMode
	{
		CONTROL_CAMERA = 0,
		CONTROL_PLAYER = 1,
		CONTROL_MAX = 2,
	};

	class Camera;
	class Window;
	class Object;
	class Light;
	class Logic
	{
	public:
		Window* window;
		Camera* camera;
		Light* light;
		Object* player;
		Object* ground;
		clock_t lastFrameTime;
		clock_t curFrameTime;
		int frameCount;
	public:
		static Logic* Create(Window* inWindow, Camera* inCamera, Light* inLight, Object* inPlayer, Object* inGround);
		static bool ProcessEvent(int eventID);
		static int GetRenderMode();
		static bool GetSkyboxState();
		static bool GetLightState();
		static bool GetShadowState();
		static bool GetProfilerState();
	public:
		void Update();
		void UpdateFPS();
		void UpdatePlayer();
		void UpdateCamera();
		void UpdateLight();
	};
}