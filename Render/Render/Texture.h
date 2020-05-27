#pragma once
#include "../Base/BaseDef.h"

namespace Engine
{
	class Color;
	class Texture
	{
	protected:
		int mWidth;
		int mHeight;
	protected:
		Texture() {}
		virtual ~Texture() {}
		Texture(const Texture&);
		Texture& operator=(const Texture&);
	public:
		int GetWidth() const { return mWidth; }
		int GetHeight() const { return mHeight; }
	};

	class Texture2D : public Texture
	{
	public:
		float* mColors;
	public:
		Texture2D();
		virtual ~Texture2D();
	public:
		Color GetPixel(int x, int y) const;
	public:
		static Texture2D* Create(const char* fileName);
	};

	class Cubemap : public Texture
	{
	public:
		float* mColors;
	public:
		Cubemap();
		virtual ~Cubemap();
	public:
		Color GetPixel(int face, int x, int y) const;
	public: 
		static Cubemap* Create(const char* fileName);
	};

	class RenderTexture : public Texture
	{
	public:
		uint32* mColorBuffers;
		float* mDepthBuffers;
	public:
		RenderTexture();
		virtual ~RenderTexture();
	public:
		Color GetPixel(int x, int y) const;
		float GetDepth(int x, int y) const;
		void SetPixel(int x, int y, uint32 color);
		void SetDepth(int x, int y, float depth);
		void Clear();
	public:
		static RenderTexture* Create(int width, int height, bool needColor, bool needDepth, void* colorBuffer = NULL);
	};
}