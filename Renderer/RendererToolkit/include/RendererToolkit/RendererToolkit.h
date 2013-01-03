/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the �Software�), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


// Comfort header putting everything within a single header, only includes the renderer header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERTOOLKIT_H__
#define __RENDERERTOOLKIT_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Forward declarations                                  ]
	//[-------------------------------------------------------]
	class IFont;
	class IRendererToolkit;


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	// RendererToolkit/IRendererToolkit.h
	class IRendererToolkit : public Renderer::RefCount<IRendererToolkit>
	{
	public:
		virtual ~IRendererToolkit();
		inline Renderer::IRenderer &getRenderer() const
		{
			return *mRenderer;
		}
	public:
		virtual IFont *createFontTexture(const char *filename, unsigned int size = 12, unsigned int resolution = 96) = 0;
	protected:
		IRendererToolkit();
		explicit IRendererToolkit(const IRendererToolkit &source);
		IRendererToolkit &operator =(const IRendererToolkit &source);
	private:
		Renderer::IRenderer *mRenderer;
	};
	typedef Renderer::SmartRefCount<IRendererToolkit> IRendererToolkitPtr;

	// RendererToolkit/IFont.h
	class IFont : public Renderer::RefCount<IFont>
	{
	public:
		enum EDrawFlags
		{
			CENTER_TEXT    = 1<<0,
			UNDERLINE_TEXT = 1<<1,
			CROSSOUT_TEXT  = 1<<2,
			MIPMAPPING     = 1<<3
		};
	public:
		virtual ~IFont();
		inline unsigned int getSize() const
		{
			return mSize;
		}
		inline unsigned int getResolution() const
		{
			return mResolution;
		}
		inline unsigned int getSizeInPixels() const
		{
			return static_cast<unsigned int>(mSize / 72.0f * mResolution);
		}
		inline unsigned int getHeightInPixels() const
		{
			return static_cast<unsigned int>(getHeight() / 72.0f * mResolution);
		}
	public:
		virtual bool setSize(unsigned int size = 12, unsigned int resolution = 96) = 0;
		virtual bool isValid() const = 0;
		virtual float getAscender() const = 0;
		virtual float getDescender() const = 0;
		virtual float getHeight() const = 0;
		virtual float getTextWidth(const char *text) = 0;
		virtual void drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX = 1.0f, float scaleY = 1.0f, float biasX = 0.0f, float biasY = 0.0f, unsigned int flags = 0) = 0;
	protected:
		IFont();
		explicit IFont(const IFont &source);
		IFont &operator =(const IFont &source);
	protected:
		unsigned int mSize;
		unsigned int mResolution;
	};
	typedef Renderer::SmartRefCount<IFont> IFontPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_H__
