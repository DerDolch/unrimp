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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/Framebuffer.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Texture2D.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	Framebuffer::Framebuffer(Direct3D9Renderer &direct3D9Renderer, unsigned int numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture) :
		IFramebuffer(direct3D9Renderer),
		mNumberOfColorTextures(numberOfColorTextures),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(depthStencilTexture),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mDirect3D9ColorSurfaces(nullptr),
		mDirect3D9DepthStencilSurface(nullptr)
	{
		// The Direct3D 9 documentation says the following about the framebuffer width and height when using multiple render targets
		//   "" (!)
		// So, in here I use the smallest width and height as the size of the framebuffer.

		// Add a reference to the used color textures
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];
			mDirect3D9ColorSurfaces = new IDirect3DSurface9*[mNumberOfColorTextures];

			// Loop through all color textures
			IDirect3DSurface9 **direct3D9ColorSurface = mDirect3D9ColorSurfaces;
			Renderer::ITexture **colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture **colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorTextures, ++direct3D9ColorSurface)
			{
				// Valid entry?
				if (nullptr != *colorTextures)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					*colorTexture = *colorTextures;
					(*colorTexture)->addReference();

					// Evaluate the color texture type
					switch ((*colorTexture)->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
						{
							// Update the framebuffer width and height if required
							Texture2D *texture2D = static_cast<Texture2D*>(*colorTexture);
							if (mWidth > texture2D->getWidth())
							{
								mWidth = texture2D->getWidth();
							}
							if (mHeight > texture2D->getHeight())
							{
								mHeight = texture2D->getHeight();
							}

							// Get the Direct3D 9 surface
							texture2D->getDirect3DTexture9()->GetSurfaceLevel(0, direct3D9ColorSurface);
							break;
						}

						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						case Renderer::ResourceType::RASTERIZER_STATE:
						case Renderer::ResourceType::DEPTH_STENCIL_STATE:
						case Renderer::ResourceType::BLEND_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
						case Renderer::ResourceType::TEXTURE_COLLECTION:
						case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
						default:
							RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 9 error: The type of the given color texture at index %d is not supported", colorTexture - colorTextures)
							*direct3D9ColorSurface = nullptr;
							break;
					}
				}
				else
				{
					*colorTexture = nullptr;
					*direct3D9ColorSurface = nullptr;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Update the framebuffer width and height if required
					Texture2D *texture2D = static_cast<Texture2D*>(mDepthStencilTexture);
					if (mWidth > texture2D->getWidth())
					{
						mWidth = texture2D->getWidth();
					}
					if (mHeight > texture2D->getHeight())
					{
						mHeight = texture2D->getHeight();
					}

					// Get the Direct3D 9 surface
					texture2D->getDirect3DTexture9()->GetSurfaceLevel(0, &mDirect3D9DepthStencilSurface);
					break;
				}

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
				default:
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: The type of the given depth stencil texture is not supported")
					break;
			}
		}

		// Validate the framebuffer width and height
		if (UINT_MAX == mWidth)
		{
			mWidth = 0;
		}
		if (UINT_MAX == mHeight)
		{
			mHeight = 0;
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	Framebuffer::~Framebuffer()
	{
		// Release the reference to the used color textures
		if (nullptr != mDirect3D9ColorSurfaces)
		{
			// Release references
			IDirect3DSurface9 **direct3D9ColorSurfacesEnd = mDirect3D9ColorSurfaces + mNumberOfColorTextures;
			for (IDirect3DSurface9 **direct3D9ColorSurface = mDirect3D9ColorSurfaces; direct3D9ColorSurface < direct3D9ColorSurfacesEnd; ++direct3D9ColorSurface)
			{
				// Valid entry?
				if (nullptr != *direct3D9ColorSurface)
				{
					(*direct3D9ColorSurface)->Release();
				}
			}

			// Cleanup
			delete [] mDirect3D9ColorSurfaces;
		}
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture **colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture **colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				// Valid entry?
				if (nullptr != *colorTexture)
				{
					(*colorTexture)->release();
				}
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDirect3D9DepthStencilSurface)
		{
			// Release reference
			mDirect3D9DepthStencilSurface->Release();
		}
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Framebuffer::setDebugName(const char *)
	{
		// In here we could assign the given debug name to all surfaces assigned to the
		// framebuffer, but this might end up within a naming chaos due to overwriting
		// possible already set names... don't do this...
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(unsigned int &width, unsigned int &height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
