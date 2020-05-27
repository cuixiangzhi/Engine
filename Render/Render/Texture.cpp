#include "Texture.h"
#include "../Math/Color.h"

namespace Engine
{
	Texture2D::Texture2D() : mColors(NULL) {}

	Texture2D::~Texture2D() { if (mColors != NULL) free(mColors); mColors = NULL; }

	Texture2D* Texture2D::Create(const char* fileName)
	{
		Texture2D* texture = new Texture2D();
		FILE* fp = NULL;
		//打开文件
		if (fopen_s(&fp, fileName, "rb") != 0) goto CREATE_FAILD;
		//读取宽高
		if (fscanf_s(fp, "%d %d", &texture->mWidth, &texture->mHeight) == 0) goto CREATE_FAILD;
		//分配内存
		texture->mColors = (float*)malloc(texture->mWidth * texture->mHeight * 4 * 4);
		//读取数据
		int colIndex = 0, rowIndex = 0, count = 0; char datatype;
		while (fscanf_s(fp, "%c", &datatype) == 1)
		{
			if (datatype == 'p')
			{
				float* colorptr = &texture->mColors[rowIndex * texture->mWidth * 4 + colIndex * 4];
				count = fscanf_s(fp, "%f %f %f %f", colorptr, colorptr + 1, colorptr + 2, colorptr + 3);
				if (count != 4) goto CREATE_FAILD;
				++colIndex; if (colIndex == texture->mWidth) { colIndex = 0; ++rowIndex; }
			}
		}
		if (rowIndex != texture->mHeight || colIndex != 0) goto CREATE_FAILD;
		fclose(fp);
		return texture;
	CREATE_FAILD:
		if(fp) fclose(fp);
		delete texture;
		return NULL;
	}

	Color Texture2D::GetPixel(int x, int y) const
	{
		float* color = &mColors[y * mWidth * 4 + x * 4];
		return Color(color[0], color[1], color[2], color[3]);
	}

	Cubemap::Cubemap() : mColors(NULL) {}

	Cubemap::~Cubemap() { if (mColors != NULL) free(mColors); mColors = NULL; }

	Cubemap* Cubemap::Create(const char* fileName)
	{
		Cubemap* cubemap = new Cubemap();
		FILE* fp = NULL; const int BUFFER_SIZE = 4; uint8 buffer[BUFFER_SIZE];
		//打开文件
		if (fopen_s(&fp, fileName, "rb") != 0) goto CREATE_FAILED;
		//读取宽高
		if (fread_s(buffer, BUFFER_SIZE, BUFFER_SIZE, 1, fp) != 1) goto CREATE_FAILED;
		cubemap->mWidth = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
		if (fread_s(buffer, BUFFER_SIZE, BUFFER_SIZE, 1, fp) != 1) goto CREATE_FAILED;
		cubemap->mHeight = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
		//分配内存
		cubemap->mColors = (float*)malloc(cubemap->mWidth * cubemap->mHeight * 4 * 4 * 6);
		//读取数据
		int colIndex = 0, rowIndex = 0, faceIndex = 0, readcount = 0; float inv255 = 1.0f / 255;
		while (!feof(fp))
		{
			if (fread_s(buffer, BUFFER_SIZE, BUFFER_SIZE, 1, fp) != 1) goto CREATE_FAILED;
			float* colorptr = &cubemap->mColors[faceIndex * cubemap->mWidth * cubemap->mHeight * 4 + rowIndex * cubemap->mWidth * 4 + colIndex * 4];
			colorptr[0] = buffer[0] * inv255; colorptr[1] = buffer[1] * inv255; colorptr[2] = buffer[2] * inv255; colorptr[3] = buffer[3] * inv255;
			++colIndex; if (colIndex == cubemap->mWidth) { colIndex = 0; ++rowIndex; }
			if (rowIndex == cubemap->mHeight) { colIndex = 0; rowIndex = 0; ++faceIndex; }
			if (faceIndex == 6) break;
		}
		if (faceIndex != 6 || colIndex != 0 || rowIndex != 0) goto CREATE_FAILED;
		fclose(fp);
		return cubemap;
	CREATE_FAILED:
		if(fp) fclose(fp);
		delete cubemap;
		return NULL;
	}

	Color Cubemap::GetPixel(int face, int x, int y) const
	{
		float* color = &mColors[face * mWidth * mHeight * 4 + y * mWidth * 4 + x * 4];
		return Color(color[0], color[1], color[2], color[3]);
	}

	RenderTexture::RenderTexture()
	{
		mColorBuffers = NULL;
		mDepthBuffers = NULL;
	}

	RenderTexture::~RenderTexture()
	{
		if (mDepthBuffers != NULL) free(mDepthBuffers);
		mColorBuffers = NULL;
		mDepthBuffers = NULL;
	}

	RenderTexture* RenderTexture::Create(int width, int height, bool needColor, bool needDepth, void* colorBuffer)
	{
		RenderTexture* texture = new RenderTexture();
		texture->mWidth = width;
		texture->mHeight = height;
		texture->mColorBuffers = (uint32*)colorBuffer;
		texture->mDepthBuffers = (float*)malloc(width * height * 4);
		return texture;
	}

	Color RenderTexture::GetPixel(int x, int y) const
	{
		if (mColorBuffers == NULL) return Color::clear;
		if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return Color::clear;
		return mColorBuffers[y * mWidth + x];
	}

	float RenderTexture::GetDepth(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return 1.0f;
		return mDepthBuffers[y * mWidth + x];
	}

	void RenderTexture::SetPixel(int x, int y, uint32 color)
	{
		if (mColorBuffers == NULL) return;
		if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return;
		mColorBuffers[y * mWidth + x] = color;
	}

	void RenderTexture::SetDepth(int x, int y, float depth)
	{
		if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return;
		mDepthBuffers[y * mWidth + x] = depth;
	}

	void RenderTexture::Clear()
	{
		for (int i = 0; i < mWidth * mHeight; ++i)
		{
			if (mColorBuffers != NULL) mColorBuffers[i] = Color::clear;
			mDepthBuffers[i] = 1.0f;
		}
	}
}