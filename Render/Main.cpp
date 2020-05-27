#include <ctime>
#include "Device/Window.h"
#include "Device/Device.h"
#include "Render/Light.h"
#include "Render/Camera.h"
#include "Render/Object.h"
#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Math/Vector3.h"
#include "Logic.h"
using namespace Engine;

Window* window = NULL;
Device* device = NULL;
Camera* camera = NULL;
Logic* logic = NULL;

Light* shadowLight = NULL;
Camera* shadowCamera = NULL;

Texture2D* playerTexture = NULL;
Texture2D* groundTexture = NULL;
Cubemap* skyboxTexture = NULL;
RenderTexture* shadowTexture = NULL;

Shader* playerShader = NULL;
Shader* groundShader = NULL;
Shader* skyboxShader = NULL;
Shader* shadowShader = NULL;

Object* player = NULL;
Object* ground = NULL;
Object* skybox = NULL;
Object* square = NULL;

CullData* cullData = NULL;

void check(bool condition, const char* content)
{
	printf(content, condition ? "failed\n" : "success\n");
	if (condition){ system("pause"); exit(0); }
}

void init()
{
	int width = 800, height = 600; float aspect = (float)width / height;
	float orthoSize = 1, fieldOfView = 90, nearPlane = 1, farPlane = 100;

	window = Window::Create("RENDER", width, height); check(window == NULL, "window create %s");
	device = Device::Create(window); check(device == NULL, "device create %s");

	camera = Camera::Create(false, aspect, fieldOfView, nearPlane, farPlane); check(camera == NULL, "camera create %s");
	shadowLight = Light::Create(); check(shadowLight == NULL, "shadowLight create %s");
	shadowCamera = Camera::Create(true, aspect, orthoSize, nearPlane, farPlane); check(shadowCamera == NULL, "shadowCamera create %s");

	playerTexture = Texture2D::Create("Data/playertexture.obj"); check(playerTexture == NULL, "playerTexture create %s");
	groundTexture = Texture2D::Create("Data/groundtexture.obj"); check(groundTexture == NULL, "groundTexture create %s");
	skyboxTexture = Cubemap::Create("Data/skyboxtexture.obj"); check(skyboxTexture == NULL, "skyboxTexture create %s");
	shadowTexture = RenderTexture::Create(256, 256, false, true); check(shadowTexture == NULL, "shadowTexture create %s");

	playerShader = NormalShader::Create(ShaderBlend::BLEND_ALPHA, ShaderZWrite::ZWRITE_OFF); check(playerShader == NULL, "playerShader create %s");
	groundShader = NormalShader::Create(ShaderBlend::BLEND_OFF, ShaderZWrite::ZWRITE_ON); check(groundShader == NULL, "groundShader create %s");
	skyboxShader = SkyboxShader::Create(); check(skyboxShader == NULL, "skyboxShader create %s");
	shadowShader = ShadowShader::Create(); check(shadowShader == NULL, "shadowShader create %s");

	ground = Object::Create("Data/groundmesh.obj", groundShader, groundTexture); check(ground == NULL, "ground create %s");
	player = Object::Create("Data/playermesh.obj", playerShader, playerTexture); check(player == NULL, "player create %s");
	skybox = Object::Create("Data/skyboxmesh.obj", skyboxShader, skyboxTexture); check(skybox == NULL, "skybox create %s");
	square = Object::Create("Data/squaremesh.obj", playerShader, playerTexture); check(square == NULL, "square create %s");

	cullData = new CullData{ { player, ground }, { ground }, { player }, shadowLight, shadowCamera, shadowShader, shadowTexture, skybox };

	logic = Logic::Create(window, camera, shadowLight, player, ground); check(device == NULL, "logic create %s");

	window->SetActive();
}

void update()
{
	while (!window->IsClosed())
	{
		window->Update();
		logic->Update();
		camera->Render(cullData);
		device->Present();
	}
}

int main(int argc, char** argv)
{
	init();
	update();
	return 0;
}