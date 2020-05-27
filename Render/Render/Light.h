#pragma once
#include "../Math/Color.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Transform.h"

namespace Engine
{
	class Light
	{
	public:
		Transform* transform;
	public:
		Color mDirectColor;
		float mDirectIntensity;
	public:
		Color mAmbientColor;
		float mAmbientIntensity;
	public:
		Light() : mDirectColor(Color::white), mDirectIntensity(1.0f), mAmbientColor(Color::white), mAmbientIntensity(0.2f) {}
	public:
		Vector3 GetDirectForward() { return transform->GetForward(); }
		Color GetDirectColor() { return mDirectColor * mDirectIntensity; }
		Color GetAmbientColor() { return mAmbientColor * mAmbientIntensity; }
	public:
		static Light* Create() 
		{ 
			Light* light = new Light();
			light->transform = new Transform();
			light->transform->SetEulerAngles({ 45, -90, 0 });
			return light;
		}
	};
}