#include "Logic.h"
#include "Base/BaseDef.h"
#include "Render/Shader.h"
#include "Render/Camera.h"
#include "Render/Object.h"
#include "Render//Light.h"
#include "Math/Transform.h"
#include "Device/Window.h"

#define VK_A 0x41
#define VK_D 0x44
#define VK_S 0x53
#define VK_W 0x57

namespace Engine
{
	static float DRAG_SPEED = 5.0f;
	static float ZOOM_SPEED = 0.5f;
	static float MOVE_SPEED = 0.5f;

	static int sRenderMode = RenderMode::RENDER_WIREFRAME;
	static int sControlMode = ControlMode::CONTROL_CAMERA;

	static bool sEnableSkybox = false;
	static bool sEnableLight = false;
	static bool sEnableShadow = false;
	static bool sEnableProfiler = false;
	static bool sEnableLightDir = false;

	static float sCameraYaw = 0;
	static float sCameraPitch = 30;
	static float sCameraDistance = 5;

	static float sPlayerForward = 0;
	static float sPlayerRight = 0;

	bool Logic::ProcessEvent(int eventID)
	{
		if (eventID == VK_SPACE) 
		{ 
			sRenderMode = RenderMode::RENDER_WIREFRAME;
			sControlMode = ControlMode::CONTROL_CAMERA;

			sEnableSkybox = false;
			sEnableLight = false;
			sEnableShadow = false;
			sEnableProfiler = false;
			sEnableLightDir = false;

			sCameraYaw = 0;
			sCameraPitch = 30;
			sCameraDistance = 5;

			sPlayerForward = 0;
			sPlayerRight = 0;
		}
		else if (eventID == VK_F1) { sRenderMode = (sRenderMode + 1) % RenderMode::RENDER_MAX; }
		else if (eventID == VK_F2) { sEnableSkybox = !sEnableSkybox; }
		else if (eventID == VK_F3) { sEnableLight = !sEnableLight; }
		else if (eventID == VK_F4) { sEnableShadow = !sEnableShadow; }
		else if (eventID == VK_F5) { sEnableLightDir = !sEnableLightDir; }
		else if (eventID == VK_F12) { sControlMode = (sControlMode + 1) % ControlMode::CONTROL_MAX; }
		else if (eventID == VK_LEFT) { sCameraYaw += DRAG_SPEED; }
		else if (eventID == VK_RIGHT) { sCameraYaw -= DRAG_SPEED; }
		else if (eventID == VK_UP) { sCameraPitch += DRAG_SPEED; }
		else if (eventID == VK_DOWN) { sCameraPitch -= DRAG_SPEED; }
		else if (eventID == VK_W && sControlMode == ControlMode::CONTROL_CAMERA) { sCameraDistance -= ZOOM_SPEED; }
		else if (eventID == VK_S && sControlMode == ControlMode::CONTROL_CAMERA) { sCameraDistance += ZOOM_SPEED; }
		else if (eventID == VK_W && sControlMode == ControlMode::CONTROL_PLAYER) { sPlayerForward = MOVE_SPEED; }
		else if (eventID == VK_A && sControlMode == ControlMode::CONTROL_PLAYER) { sPlayerRight = -MOVE_SPEED; }
		else if (eventID == VK_S && sControlMode == ControlMode::CONTROL_PLAYER) { sPlayerForward = -MOVE_SPEED; }
		else if (eventID == VK_D && sControlMode == ControlMode::CONTROL_PLAYER) { sPlayerRight = MOVE_SPEED; }
		else return false;
		return true;
	}

	int Logic::GetRenderMode() { return sRenderMode; }

	bool Logic::GetSkyboxState() { return sEnableSkybox; }

	bool Logic::GetLightState() { return sEnableLight; }

	bool Logic::GetShadowState() { return sEnableShadow; }

	bool Logic::GetProfilerState() { return sEnableProfiler; }

	Logic* Logic::Create(Window* inWindow, Camera* inCamera, Light* inLight, Object* inPlayer, Object* inGround)
	{
		Logic* logic = new Logic();
		logic->window = inWindow;
		logic->camera = inCamera;
		logic->light = inLight;
		logic->player = inPlayer;
		logic->ground = inGround;
		logic->lastFrameTime = 0;
		system("cls");
		return logic;
	}

	void Logic::UpdateFPS()
	{
		++frameCount;
		curFrameTime = clock();
		if (curFrameTime - lastFrameTime < CLOCKS_PER_SEC) return;
		int deltaTime = curFrameTime - lastFrameTime;
		int fps = CLOCKS_PER_SEC * frameCount / deltaTime;
		int fms = deltaTime / frameCount;
		window->SetCursor(0, 0);
		printf("FPS:%d TIME:%dms    \n", fps, fms);
		printf("SPACE 全部重置\n");
		printf("F1 切换渲染模式\n");
		printf("F2 开启天空盒\n");
		printf("F3 开启平行光\n");
		printf("F4 开启阴影\n");
		printf("F5 光源保持在镜头右侧\n");
		printf("镜头控制 WS↑↓←→\n");
		lastFrameTime = curFrameTime;
		frameCount = 0;
	}

	void Logic::UpdatePlayer()
	{
		ground->transform->SetLocalScale({ 10, 0.5f, 10 });
		Vector3 forwardMove = camera->transform->GetForward() * sPlayerForward;
		Vector3 rightMove = camera->transform->GetRight() * sPlayerRight;
		sPlayerForward = 0.0f; sPlayerRight = 0.0f;
		Vector3 position = player->transform->GetPosition() + forwardMove + rightMove;
		player->transform->SetLocalScale({ 2, 2, 2 });
		player->transform->SetPosition({ position.x, 1, position.z });
	}

	void Logic::UpdateCamera()
	{
		camera->transform->SetEulerAngles({ sCameraPitch, sCameraYaw, 0 });
		camera->transform->SetPosition(player->transform->GetPosition() - camera->transform->GetForward() * sCameraDistance);
	}

	void Logic::UpdateLight()
	{
		if (sEnableLightDir)
		{
			light->transform->SetEulerAngles({ sCameraPitch, sCameraYaw - 90, 0 });
		}
	}

	void Logic::Update()
	{
		UpdateFPS();
		UpdatePlayer();
		UpdateCamera();
		UpdateLight();
	}
}