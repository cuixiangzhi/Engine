#include "Device.h"
#include "Window.h"
#include "../Render/Mesh.h"
#include "../Render/Shader.h"
#include "../Render/Object.h"
#include "../Render/Texture.h"
#include "../Logic.h"

namespace Engine
{
	Device* Device::mDeviceInstance = NULL;

	Device::Device()
	{
		mWindow = NULL;
		mShader = NULL;
		mDefaultTarget = NULL;
		mActiveTarget = NULL;
	}

	Device::~Device()
	{
		mWindow = NULL;
		mShader = NULL;
		mActiveTarget = NULL;
		if (mDefaultTarget != NULL)
		{
			delete mDefaultTarget;
			mDefaultTarget = NULL;
		}
	}

	Device* Device::Create(Window* window)
	{
		Device* device = new Device();
		device->mWindow = window;
		device->mDefaultTarget = RenderTexture::Create(window->GetWidth(), window->GetHeight(), true, true, window->GetBitmapBuffer());
		device->mActiveTarget = device->mDefaultTarget;
		if (device->mActiveTarget == NULL) goto CREATE_FAILED;
		mDeviceInstance = device;
		return device;
	CREATE_FAILED:
		delete device;
		return NULL;
	}

	void Device::Clear(RenderTexture* renderTarget)
	{
		mActiveTarget = renderTarget != NULL ? renderTarget : mDefaultTarget;
		mActiveTarget->Clear();
	}

	void Device::Present()
	{
		if (mWindow != NULL) mWindow->SwapBuffer();
	}

	int Device::GetWidth() const
	{
		return mActiveTarget->GetWidth();
	}

	int Device::GetHeight() const
	{
		return mActiveTarget->GetHeight();
	}

	void Device::DrawObject(Object* object, Shader* shader)
	{
		mShader = shader;
		for (int i = 0; i < object->mesh->faceCount; ++i)
		{
			auto& vertexData = object->mesh->vertexData;
			auto& faceData = object->mesh->faceData;
			DrawTriangle(vertexData[faceData[i].v0], vertexData[faceData[i].v1], vertexData[faceData[i].v2]);
		}
	}

	void Device::DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		//������ɫ��
		Fragment& clipV0 = mShader->vertex(v0);
		Fragment& clipV1 = mShader->vertex(v1);
		Fragment& clipV2 = mShader->vertex(v2);

		//CVV�ü�
		vector<Fragment> input = { clipV0, clipV1, clipV2 }, output = {};
		mShader->cvvcull(input, output);

		//�����ü�������͹�����
		for (int i = 2; i < (int)input.size(); ++i)
		{
			clipV0 = input[0]; clipV1 = input[i - 1]; clipV2 = input[i];

			//͸�ӳ���
			mShader->division(clipV0, clipV1, clipV2);

			//�����޳�
			if (!mShader->backcull(clipV0, clipV1, clipV2)) continue;

			//�ӿڱ任
			mShader->viewport(clipV0, clipV1, clipV2);

			//��դ��
			switch (Logic::GetRenderMode())
			{
			case RenderMode::RENDER_WIREFRAME:
				if (i == 2) DrawLineDDA(clipV0, clipV1);
				if (i == input.size() - 1) DrawLineDDA(clipV2, clipV0);
				DrawLineDDA(clipV1, clipV2);
				break;
			case RenderMode::RENDER_SHADED:
				DrawTriangleScanline(clipV0, clipV1, clipV2);
				break;
			}
		}
	}

	void Device::DrawTriangleScanline(Fragment v0, Fragment v1, Fragment v2)
	{
		//ȷ����Ļ����Ϊ����
		v0.position.y = Math::Floorf(v0.position.y);
		v1.position.y = Math::Floorf(v1.position.y);
		v2.position.y = Math::Floorf(v2.position.y);

		//����ˮƽ��
		if (v0.position.y == v1.position.y && v0.position.y == v2.position.y) return;

		//�ӵ͵�������
		if (v0.position.y > v1.position.y) { swap(v0, v1); }
		if (v0.position.y > v2.position.y) { swap(v0, v2); }
		if (v1.position.y > v2.position.y) { swap(v1, v2); }

		bool upTriangle = v0.position.y == v1.position.y;
		bool downTriangle = v1.position.y == v2.position.y;
		if (upTriangle || downTriangle)
		{
			Fragment lbv, ltv, rbv, rtv, linel, liner;
			if (upTriangle)
			{
				if (v0.position.x > v1.position.x) { swap(v0, v1); }
				lbv = v0; ltv = v2; rbv = v1; rtv = v2;
			}
			else if (downTriangle)
			{
				if (v1.position.x > v2.position.x) { swap(v1, v2); }
				lbv = v0; ltv = v1; rbv = v0; rtv = v2;
			}
			//�������ε������δ���,ֻ�Ա߽�������,�����ۻ�
			float hPercent, wPercent, linexl, linexr, boty = Math::Floorf(lbv.position.y), topy = Math::Floorf(ltv.position.y);
			for (float y = boty; y <= topy; ++y)
			{
				linexl = Math::Floorf(linel.position.x);
				linexr = Math::Floorf(liner.position.x);
				for (float x = linexl; x < linexr; ++x)
				{
					wPercent = linexl == linexr ? 0.0f : (x - linexl) / (linexr - linexl);
					auto v = Fragment::Lerp(linel, liner, wPercent);
					v.position.x = x; v.position.y = y;
					DrawPixel(v);
				}
				hPercent = topy == boty ? 1.0f : (y - boty) / (topy - boty);
				linel = Fragment::Lerp(lbv, ltv, hPercent);
				liner = Fragment::Lerp(rbv, rtv, hPercent);
			}
		}
		else
		{
			//�������ηֳ�����������
			Fragment newv = Fragment::Lerp(v0, v2, (v1.position.y - v0.position.y) / (v2.position.y - v0.position.y));
			DrawTriangleScanline(v0, v1, newv);
			DrawTriangleScanline(newv, v1, v2);
		}
	}

	void Device::DrawLineDDA(Fragment v0, Fragment v1)
	{
		//ȷ����Ļ����Ϊ����
		v0.position.x = Math::Floorf(v0.position.x); v0.position.y = Math::Floorf(v0.position.y);
		v1.position.x = Math::Floorf(v1.position.x); v1.position.y = Math::Floorf(v1.position.y);

		if (v0.position.x == v1.position.x && v0.position.y == v1.position.y)
		{
			//һ����
			DrawPixel(v0);
		}
		else if (v0.position.x == v1.position.x)
		{
			//��ֱ��
			if (v0.position.y > v1.position.y) { swap(v0, v1); }
			for (float y = v0.position.y; y <= v1.position.y; ++y)
			{
				DrawPixel(Fragment::Lerp(v0, v1, (y - v0.position.y) / (v1.position.y - v0.position.y)));
			}
		}
		else if (v0.position.y == v1.position.y)
		{
			//ˮƽ��
			if (v0.position.x > v1.position.x) { swap(v0, v1); }
			for (float x = v0.position.x; x <= v1.position.x; ++x)
			{
				DrawPixel(Fragment::Lerp(v0, v1, (x - v0.position.x) / (v1.position.x - v0.position.x)));
			}
		}
		else
		{
			float k = (v1.position.y - v0.position.y) / (v1.position.x - v0.position.x);
			if (k >= -1 && k <= 1)
			{
				//|k|<=1 ��������x����Ϊ1,y����Ϊk
				if (v0.position.x > v1.position.x) { swap(v0, v1); }
				float y = v0.position.y, dy = (v1.position.y - v0.position.y) / (v1.position.x - v0.position.x);
				for (float x = v0.position.x; x <= v1.position.x; x += 1.0f, y += dy)
				{
					DrawPixel(Fragment::Lerp(v0, v1, (x - v0.position.x) / (v1.position.x - v0.position.x)));
				}
			}
			else
			{
				//|k|>1 ��������y����Ϊ1,x����Ϊ1/k
				if (v0.position.y > v1.position.y) { swap(v0, v1); }
				float x = v0.position.x, dx = (v1.position.x - v0.position.x) / (v1.position.y - v0.position.y);
				for (float y = v0.position.y; y <= v1.position.y; y += 1.0f, x += dx)
				{
					DrawPixel(Fragment::Lerp(v0, v1, (y - v0.position.y) / (v1.position.y - v0.position.y)));
				}
			}
		}
	}

	void Device::DrawPixel(const Fragment& v)
	{
		int x = (int)Math::Floorf(v.position.x);
		int y = (int)Math::Floorf(v.position.y);
		//��Ȳ���
		if (!mShader->depthtest(v.position.z, mActiveTarget->GetDepth(x, y))) return;
		//������ɫ��
		Color color = mShader->fragment(v);
		//�޸���Ⱥ���ɫ
		mShader->updatecolor(mActiveTarget, x, y, color);
		mShader->updatedepth(mActiveTarget, x, y, v.position.z);
	}
}