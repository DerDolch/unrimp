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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/OpenGLES2Debug.h"	// For "OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/Program.h"
#include "OpenGLES2Renderer/Texture2D.h"
#include "OpenGLES2Renderer/BlendState.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/IndexBuffer.h"
#include "OpenGLES2Renderer/SamplerState.h"
#include "OpenGLES2Renderer/VertexBuffer.h"
#include "OpenGLES2Renderer/VertexArrayVao.h"
#include "OpenGLES2Renderer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/SwapChain.h"
#include "OpenGLES2Renderer/Framebuffer.h"
#include "OpenGLES2Renderer/Texture2DArray.h"
#include "OpenGLES2Renderer/RasterizerState.h"
#include "OpenGLES2Renderer/DepthStencilState.h"
#include "OpenGLES2Renderer/TextureCollection.h"
#include "OpenGLES2Renderer/SamplerStateCollection.h"
#include "OpenGLES2Renderer/ContextRuntimeLinking.h"
#include "OpenGLES2Renderer/ShaderLanguageGlsl.h"
#ifndef OPENGLES2RENDERER_NO_CG
	#include "OpenGLES2Renderer/ShaderLanguageCg.h"
#endif


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLES2RENDERER_EXPORTS
	#define OPENGLES2RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLES2RENDERER_API_EXPORT
#endif
OPENGLES2RENDERER_API_EXPORT Renderer::IRenderer *createOpenGLES2RendererInstance(handle nativeWindowHandle)
{
	return new OpenGLES2Renderer::OpenGLES2Renderer(nativeWindowHandle);
}
#undef OPENGLES2RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	OpenGLES2Renderer::OpenGLES2Renderer(handle nativeWindowHandle) :
		mContext(new ContextRuntimeLinking(nativeWindowHandle)),
		mShaderLanguageGlsl(nullptr),
		#ifndef OPENGLES2RENDERER_NO_CG
			mShaderLanguageCg(nullptr),
		#endif
		mDefaultSamplerState(nullptr),
		mVertexArray(nullptr),
		mOpenGLES2PrimitiveTopology(0xFFFF),	// Unknown default setting
		mDefaultRasterizerState(nullptr),
		mRasterizerState(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mDefaultDepthStencilState(nullptr),
		mDepthStencilState(nullptr),
		mDefaultBlendState(nullptr),
		mBlendState(nullptr),
		mOpenGLES2Program(0)
	{
		// Initialize the context
		if (mContext->initialize(0))
		{
			// Create the default state objects
			mDefaultSamplerState	  = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());
			mDefaultRasterizerState	  = createRasterizerState(Renderer::IRasterizerState::getDefaultRasterizerState());
			mDefaultDepthStencilState = createDepthStencilState(Renderer::IDepthStencilState::getDefaultDepthStencilState());
			mDefaultBlendState		  = createBlendState(Renderer::IBlendState::getDefaultBlendState());

			// Initialize the capabilities
			initializeCapabilities();

			// Add references to the default state objects and set them
			if (nullptr != mDefaultRasterizerState)
			{
				mDefaultRasterizerState->addReference();
				rsSetState(mDefaultRasterizerState);
			}
			if (nullptr != mDefaultDepthStencilState)
			{
				mDefaultDepthStencilState->addReference();
				omSetDepthStencilState(mDefaultDepthStencilState);
			}
			if (nullptr != mDefaultBlendState)
			{
				mDefaultBlendState->addReference();
				omSetBlendState(mDefaultBlendState);
			}
			if (nullptr != mDefaultSamplerState)
			{
				mDefaultSamplerState->addReference();
				// TODO(co) Set default sampler states
			}

			// Create a main swap chain instance?
			if (NULL_HANDLE != nativeWindowHandle)
			{
				// Create a main swap chain instance
				mMainSwapChain = static_cast<SwapChain*>(createSwapChain(nativeWindowHandle));
				RENDERER_SET_RESOURCE_DEBUG_NAME(mMainSwapChain, "Main swap chain")
				mMainSwapChain->addReference();	// Internal renderer reference

				// By default, set the created main swap chain as the currently used render target
				omSetRenderTarget(mMainSwapChain);
			}
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	OpenGLES2Renderer::~OpenGLES2Renderer()
	{
		// Release instances
		if (nullptr != mDefaultRasterizerState)
		{
			mDefaultRasterizerState->release();
			mDefaultRasterizerState = nullptr;
		}
		if (nullptr != mMainSwapChain)
		{
			mMainSwapChain->release();
			mMainSwapChain = nullptr;
		}
		if (nullptr != mRenderTarget)
		{
			mRenderTarget->release();
			mRenderTarget = nullptr;
		}
		if (nullptr != mDefaultDepthStencilState)
		{
			mDefaultDepthStencilState->release();
			mDefaultDepthStencilState = nullptr;
		}
		if (nullptr != mDefaultBlendState)
		{
			mDefaultBlendState->release();
			mDefaultBlendState = nullptr;
		}
		if (nullptr != mDefaultSamplerState)
		{
			mDefaultSamplerState->release();
			mDefaultSamplerState = nullptr;
		}

		// Set no vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			iaSetVertexArray(nullptr);
		}

		// Set no rasterizer state reference, in case we have one
		if (nullptr != mRasterizerState)
		{
			rsSetState(nullptr);
		}

		// Set no depth stencil reference, in case we have one
		if (nullptr != mDepthStencilState)
		{
			omSetDepthStencilState(nullptr);
		}

		// Set no blend state reference, in case we have one
		if (nullptr != mBlendState)
		{
			omSetBlendState(nullptr);
		}

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		#ifndef OPENGLES2RENDERER_NO_CG
			// Release the Cg shader language instance, in case we have one
			if (nullptr != mShaderLanguageCg)
			{
				mShaderLanguageCg->release();
			}
		#endif

		// Release the GLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageGlsl)
		{
			mShaderLanguageGlsl->release();
		}

		// Destroy the context instance
		delete mContext;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char *OpenGLES2Renderer::getName() const
	{
		return "OpenGLES2";
	}

	bool OpenGLES2Renderer::isInitialized() const
	{
		// Is the context initialized?
		return (EGL_NO_CONTEXT != mContext->getEGLContext());
	}

	Renderer::ISwapChain *OpenGLES2Renderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	unsigned int OpenGLES2Renderer::getNumberOfShaderLanguages() const
	{
		#ifndef OPENGLES2RENDERER_NO_CG
			// "GL_EXT_Cg_shader required" for Cg support
			return mContext->getExtensions().isGL_EXT_Cg_shader() ? 2u : 1u;
		#else
			return 1;
		#endif
	}

	const char *OpenGLES2Renderer::getShaderLanguageName(unsigned int index) const
	{
		// Evaluate the provided index
		switch (index)
		{
			case 0:
				return ShaderLanguageGlsl::NAME;

			#ifndef OPENGLES2RENDERER_NO_CG
				case 1:
					// "GL_EXT_Cg_shader" required for Cg support
					return mContext->getExtensions().isGL_EXT_Cg_shader() ? ShaderLanguageCg::NAME : nullptr;
			#endif
		}

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage *OpenGLES2Renderer::getShaderLanguage(const char *shaderLanguageName)
	{
		// In case "shaderLanguage" is a null pointer, use the default shader language
		if (nullptr != shaderLanguageName)
		{
			// Optimization: Check for shader language name pointer match, first
			if (shaderLanguageName == ShaderLanguageGlsl::NAME || !stricmp(shaderLanguageName, ShaderLanguageGlsl::NAME))
			{
				// If required, create the GLSL shader language instance right now
				if (nullptr == mShaderLanguageGlsl)
				{
					mShaderLanguageGlsl = new ShaderLanguageGlsl(*this);
					mShaderLanguageGlsl->addReference();	// Internal renderer reference
				}

				// Return the shader language instance
				return mShaderLanguageGlsl;
			}
			#ifndef OPENGLES2RENDERER_NO_CG
				else if (ShaderLanguageCg::NAME == shaderLanguageName || !stricmp(shaderLanguageName, ShaderLanguageCg::NAME))
				{
					// If required, create the Cg shader language instance right now
					// -> "GL_EXT_Cg_shader" required for Cg support
					if (nullptr == mShaderLanguageCg && mContext->getExtensions().isGL_EXT_Cg_shader())
					{
						mShaderLanguageCg = new ShaderLanguageCg(*this);
						mShaderLanguageCg->addReference();	// Internal renderer reference
					}

					// Return the shader language instance
					return mShaderLanguageCg;
				}
			#endif

			// Error!
			return nullptr;
		}

		// Return the GLSL shader language instance as default
		return getShaderLanguage(ShaderLanguageGlsl::NAME);
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain *OpenGLES2Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *OpenGLES2Renderer::createFramebuffer(unsigned int numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IVertexBuffer *OpenGLES2Renderer::createVertexBuffer(unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *OpenGLES2Renderer::createIndexBuffer(unsigned int numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::ITextureBuffer *OpenGLES2Renderer::createTextureBuffer(unsigned int, Renderer::TextureFormat::Enum, const void *, Renderer::BufferUsage::Enum)
	{
		// OpenGL ES 2 has no texture buffer support
		return nullptr;
	}

	Renderer::ITexture2D *OpenGLES2Renderer::createTexture2D(unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags, Renderer::TextureUsage::Enum)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 2 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(*this, width, height, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *OpenGLES2Renderer::createTexture2DArray(unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags, Renderer::TextureUsage::Enum)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 2 has no texture usage indication

		// Check whether or not the given texture dimension is valid, "GL_EXT_texture_array" extension required
		if (width > 0 && height > 0 && numberOfSlices > 0 && mContext->getExtensions().isGL_EXT_texture_array())
		{
			return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::IRasterizerState *OpenGLES2Renderer::createRasterizerState(const Renderer::RasterizerState &rasterizerState)
	{
		return new RasterizerState(*this, rasterizerState);
	}

	Renderer::IDepthStencilState *OpenGLES2Renderer::createDepthStencilState(const Renderer::DepthStencilState &depthStencilState)
	{
		return new DepthStencilState(*this, depthStencilState);
	}

	Renderer::IBlendState *OpenGLES2Renderer::createBlendState(const Renderer::BlendState &blendState)
	{
		return new BlendState(*this, blendState);
	}

	Renderer::ISamplerState *OpenGLES2Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}

	Renderer::ITextureCollection *OpenGLES2Renderer::createTextureCollection(unsigned int numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *OpenGLES2Renderer::createSamplerStateCollection(unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool OpenGLES2Renderer::map(Renderer::IResource &, unsigned int, Renderer::MapType::Enum, unsigned int, Renderer::MappedSubresource &)
	{
		// TODO(co) Implement me
		return false;
	}

	void OpenGLES2Renderer::unmap(Renderer::IResource &, unsigned int)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::setProgram(Renderer::IProgram *program)
	{
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// Backup OpenGL ES 2 program identifier
			mOpenGLES2Program = static_cast<Program*>(program)->getOpenGLES2Program();

			// Bind the program
			glUseProgram(mOpenGLES2Program);
		}
		else
		{
			// Unbind the program
			glUseProgram(0);

			// Reset OpenGL ES 2 program identifier
			mOpenGLES2Program = 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

				// Is GL_OES_vertex_array_object there?
				if (mContext->getExtensions().isGL_OES_vertex_array_object())
				{
					// Release the vertex array reference, in case we have one
					if (nullptr != mVertexArray)
					{
						// Release reference
						mVertexArray->release();
					}

					// Set new vertex array and add a reference to it
					mVertexArray = static_cast<VertexArray*>(vertexArray);
					mVertexArray->addReference();

					// Bind OpenGL ES 2 vertex array
					glBindVertexArrayOES(static_cast<VertexArrayVao*>(mVertexArray)->getOpenGLES2VertexArray());
				}
				else
				{
					// Release the vertex array reference, in case we have one
					if (nullptr != mVertexArray)
					{
						// Disable OpenGL ES 2 vertex attribute arrays
						static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLES2VertexAttribArrays();

						// Release reference
						mVertexArray->release();
					}

					// Set new vertex array and add a reference to it
					mVertexArray = static_cast<VertexArray*>(vertexArray);
					mVertexArray->addReference();

					// Enable OpenGL ES 2 vertex attribute arrays
					static_cast<VertexArrayNoVao*>(mVertexArray)->enableOpenGLES2VertexAttribArrays();
				}
			}
			else
			{
				// Release the vertex array reference, in case we have one
				if (nullptr != mVertexArray)
				{
					// Disable OpenGL ES 2 vertex attribute arrays - Is the "GL_OES_vertex_array_object" extension there?
					if (mContext->getExtensions().isGL_OES_vertex_array_object())
					{
						// Unbind OpenGL ES 2 vertex array
						glBindVertexArrayOES(0);
					}
					else
					{
						// Traditional version
						static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLES2VertexAttribArrays();
					}

					// Release reference
					mVertexArray->release();
					mVertexArray = nullptr;
				}
			}
		}
	}

	void OpenGLES2Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Map and backup the set OpenGL ES 2 primitive topology
		mOpenGLES2PrimitiveTopology = Mapping::getOpenGLES2Type(primitiveTopology);
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::vsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// In OpenGL ES 2, all shaders share the same texture units
		fsSetTexture(unit, texture);
	}

	void OpenGLES2Renderer::vsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// In OpenGL ES 2, all shaders share the same texture units
		fsSetTextureCollection(startUnit, textureCollection);
	}

	void OpenGLES2Renderer::vsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// In OpenGL ES 2, all shaders share the same texture units
		fsSetSamplerState(unit, samplerState);
	}

	void OpenGLES2Renderer::vsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// In OpenGL ES 2, all shaders share the same texture units
		fsSetSamplerStateCollection(startUnit, samplerStateCollection);
	}

	void OpenGLES2Renderer::vsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// OpenGL ES 2 has no uniform buffer support

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::tcsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// OpenGL ES 2 has no tessellation control shader support

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void OpenGLES2Renderer::tcsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// OpenGL ES 2 has no tessellation control shader support

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void OpenGLES2Renderer::tcsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// OpenGL ES 2 has no tessellation control shader support

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void OpenGLES2Renderer::tcsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// OpenGL ES 2 has no tessellation control shader support

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void OpenGLES2Renderer::tcsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// OpenGL ES 2 has no tessellation control shader support
		// OpenGL ES 2 has no uniform buffer support

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::tesSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// OpenGL ES 2 has no tessellation evaluation shader support

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void OpenGLES2Renderer::tesSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// OpenGL ES 2 has no tessellation evaluation shader support

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void OpenGLES2Renderer::tesSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// OpenGL ES 2 has no tessellation evaluation shader support

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void OpenGLES2Renderer::tesSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// OpenGL ES 2 has no tessellation evaluation shader support

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void OpenGLES2Renderer::tesSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// OpenGL ES 2 has no tessellation evaluation shader support
		// OpenGL ES 2 has no uniform buffer support

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::gsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// OpenGL ES 2 has no geometry shader support

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void OpenGLES2Renderer::gsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// OpenGL ES 2 has no geometry shader support

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void OpenGLES2Renderer::gsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// OpenGL ES 2 has no geometry shader support

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void OpenGLES2Renderer::gsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// OpenGL ES 2 has no geometry shader support

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void OpenGLES2Renderer::gsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// OpenGL ES 2 has no geometry shader support
		// OpenGL ES 2 has no uniform buffer support

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::rsSetViewports(unsigned int numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// In OpenGL ES 2, the origin of the viewport is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			unsigned int renderTargetWidth =  0;
			unsigned int renderTargetHeight = 0;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL ES 2 viewport
			// -> OpenGL ES 2 supports only one viewport
		#ifndef RENDERER_NO_DEBUG
			if (numberOfViewports > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 supports only one viewport")
			}
		#endif
			glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
			glDepthRangef(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
		}
	}

	void OpenGLES2Renderer::rsSetScissorRectangles(unsigned int numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// In OpenGL ES 2, the origin of the scissor rectangle is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 9 & 10 & 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			unsigned int renderTargetWidth =  0;
			unsigned int renderTargetHeight = 0;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL ES 2 scissor rectangle
		#ifndef RENDERER_NO_DEBUG
			if (numberOfScissorRectangles > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 supports only one scissor rectangle")
			}
		#endif
			const GLsizei width  = scissorRectangles->bottomRightX - scissorRectangles->topLeftX;
			const GLsizei height = scissorRectangles->bottomRightY - scissorRectangles->topLeftY;
			glScissor(static_cast<GLint>(scissorRectangles->topLeftX), static_cast<GLint>(renderTargetHeight - scissorRectangles->topLeftY - height), width, height);
		}
	}

	void OpenGLES2Renderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		// New rasterizer state?
		if (mRasterizerState != rasterizerState)
		{
			// Set a rasterizer state?
			if (nullptr != rasterizerState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)

				// Release the rasterizer state reference, in case we have one
				if (nullptr != mRasterizerState)
				{
					mRasterizerState->release();
				}

				// Set new rasterizer state and add a reference to it
				mRasterizerState = static_cast<RasterizerState*>(rasterizerState);
				mRasterizerState->addReference();

				// Set the OpenGL ES 2 rasterizer states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mRasterizerState->setOpenGLES2RasterizerStates();
			}
			else
			{
				// Set the default rasterizer state
				if (nullptr != mDefaultRasterizerState)
				{
					rsSetState(mDefaultRasterizerState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the rasterizer state reference, in case we have one
					if (nullptr != mRasterizerState)
					{
						// Release reference
						mRasterizerState->release();
						mRasterizerState = nullptr;
					}
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Fragment-shader (FS) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::fsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));

			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
					glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLES2Texture());
					break;

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					// No extension check required, if we in here we already know it must exist
					glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLES2Texture());
					break;

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
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
					// Not handled in here
					break;
			}

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
		}
		else
		{
			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			glActiveTexture(GL_TEXTURE0 + unit);

			// Unbind the texture at the given texture unit
			glBindTexture(GL_TEXTURE_2D, 0);

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
		}
	}

	void OpenGLES2Renderer::fsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// Loop through all textures within the given texture collection
			// -> "GL_TEXTURE0" is the first texture unit, while nUnit we received is zero based
			Renderer::ITexture **currentTexture = static_cast<TextureCollection*>(textureCollection)->getTextures();
			Renderer::ITexture **textureEnd	    = currentTexture + static_cast<TextureCollection*>(textureCollection)->getNumberOfTextures();
			for (unsigned int unit = static_cast<GLenum>(GL_TEXTURE0 + startUnit); currentTexture < textureEnd; ++currentTexture, ++unit)
			{
				// Get the current texture
				Renderer::ITexture *texture = *currentTexture;

				// TODO(co) Some security checks might be wise *maximum number of texture units*
				glActiveTexture(unit);

				// Set a texture at that unit?
				if (nullptr != texture)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the texture collection

					// Evaluate the texture
					switch (texture->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
							glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLES2Texture());
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							// No extension check required, if we in here we already know it must exist
							glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLES2Texture());
							break;

						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
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
							// Not handled in here
							break;
					}
				}
				else
				{
					// Unbind the texture at the given texture unit
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
		}
	}

	void OpenGLES2Renderer::fsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			glActiveTexture(GL_TEXTURE0 + unit);

			// Set the OpenGL ES 2 sampler states
			static_cast<SamplerState*>(samplerState)->setOpenGLES2SamplerStates();

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
		}
		else
		{
			// Set the default sampler state
			if (nullptr != mDefaultSamplerState)
			{
				fsSetSamplerState(unit, mDefaultSamplerState);
			}
			else
			{
				// Fallback in case everything goes wrong

				// TODO(co) Implement me
			}
		}
	}

	void OpenGLES2Renderer::fsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// Loop through all sampler states within the given sampler state collection
			Renderer::ISamplerState **currentSamplerState = static_cast<SamplerStateCollection*>(samplerStateCollection)->getSamplerStates();
			Renderer::ISamplerState **samplerStateEnd	  = currentSamplerState + static_cast<SamplerStateCollection*>(samplerStateCollection)->getNumberOfSamplerStates();
			for (unsigned int unit = startUnit; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++unit)
			{
				// Get the current sampler state
				Renderer::ISamplerState *samplerState = *currentSamplerState;

				// Set a sampler state at that unit?
				if (nullptr != samplerState)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the sampler state collection

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// -> "GL_TEXTURE0" is the first texture unit, while nUnit we received is zero based
					glActiveTexture(GL_TEXTURE0 + unit);

					// Set the OpenGL ES 2 sampler states
					static_cast<SamplerState*>(samplerState)->setOpenGLES2SamplerStates();
				}
				else
				{
					// Set the default sampler state
					if (nullptr != mDefaultSamplerState)
					{
						fsSetSamplerState(unit, mDefaultSamplerState);
					}
					else
					{
						// Fallback in case everything goes wrong

						// TODO(co) Implement me
					}
				}
			}

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
		}
	}

	void OpenGLES2Renderer::fsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// OpenGL ES 2 has no uniform buffer support

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *OpenGLES2Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void OpenGLES2Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL ES 2 framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// We do not render into a OpenGL ES 2 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

					// Release the reference
					mRenderTarget->release();
				}

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// Evaluate the render target type
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// TODO(co) Implement me
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the OpenGL ES 2 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Bind the OpenGL ES 2 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getOpenGLES2Framebuffer());

						// Define the OpenGL buffers to draw into, "GL_NV_draw_buffers"-extension required
						if (mContext->getExtensions().isGL_NV_draw_buffers())
						{
							// "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
							static const GLenum OPENGL_DRAW_BUFFER[16] =
							{
								GL_COLOR_ATTACHMENT0_NV,  GL_COLOR_ATTACHMENT1_NV,  GL_COLOR_ATTACHMENT2_NV,  GL_COLOR_ATTACHMENT3_NV,
								GL_COLOR_ATTACHMENT4_NV,  GL_COLOR_ATTACHMENT5_NV,  GL_COLOR_ATTACHMENT6_NV,  GL_COLOR_ATTACHMENT7_NV,
								GL_COLOR_ATTACHMENT8_NV,  GL_COLOR_ATTACHMENT9_NV,  GL_COLOR_ATTACHMENT10_NV, GL_COLOR_ATTACHMENT11_NV,
								GL_COLOR_ATTACHMENT12_NV, GL_COLOR_ATTACHMENT13_NV, GL_COLOR_ATTACHMENT14_NV, GL_COLOR_ATTACHMENT15_NV
							};
							glDrawBuffersNV(static_cast<GLsizei>(framebuffer->getNumberOfColorTextures()), OPENGL_DRAW_BUFFER);
						}
						break;
					}

					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::INDEX_BUFFER:
					case Renderer::ResourceType::VERTEX_BUFFER:
					case Renderer::ResourceType::UNIFORM_BUFFER:
					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::TEXTURE_2D:
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
						// Not handled in here
						break;
				}
			}
			else
			{
				// Evaluate the render target type
				if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType())
				{
					// We do not render into a OpenGL ES 2 framebuffer
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

				// TODO(co) Set no active render target

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->release();
					mRenderTarget = nullptr;
				}
			}
		}
	}

	void OpenGLES2Renderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		// New depth stencil state?
		if (mDepthStencilState != depthStencilState)
		{
			// Set a depth stencil state?
			if (nullptr != depthStencilState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)

				// Release the depth stencil state reference, in case we have one
				if (nullptr != mDepthStencilState)
				{
					mDepthStencilState->release();
				}

				// Set new depth stencil state and add a reference to it
				mDepthStencilState = static_cast<DepthStencilState*>(depthStencilState);
				mDepthStencilState->addReference();

				// Set the OpenGL ES 2 depth stencil states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mDepthStencilState->setOpenGLES2DepthStencilStates();
			}
			else
			{
				// Set the default depth stencil state
				if (nullptr != mDefaultDepthStencilState)
				{
					omSetDepthStencilState(mDefaultDepthStencilState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the depth stencil state reference, in case we have one
					if (nullptr != mDepthStencilState)
					{
						// Release reference
						mDepthStencilState->release();
						mDepthStencilState = nullptr;
					}
				}
			}
		}
	}

	void OpenGLES2Renderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		// New blend state?
		if (mBlendState != blendState)
		{
			// Set a blend state?
			if (nullptr != blendState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)

				// Release the blend state reference, in case we have one
				if (nullptr != mBlendState)
				{
					mBlendState->release();
				}

				// Set new blend state and add a reference to it
				mBlendState = static_cast<BlendState*>(blendState);
				mBlendState->addReference();

				// Set the OpenGL ES 2 blend states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mBlendState->setOpenGLES2BlendStates();
			}
			else
			{
				// Set the default blend state
				if (nullptr != mDefaultBlendState)
				{
					omSetBlendState(mDefaultBlendState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the blend state reference, in case we have one
					if (nullptr != mBlendState)
					{
						// Release reference
						mBlendState->release();
						mBlendState = nullptr;
					}
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::clear(unsigned int flags, const float color[4], float z, unsigned int stencil)
	{
		// Get API flags
		unsigned int flagsAPI = 0;
		if (flags & Renderer::ClearFlag::COLOR)
		{
			flagsAPI |= GL_COLOR_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::DEPTH)
		{
			flagsAPI |= GL_DEPTH_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::STENCIL)
		{
			flagsAPI |= GL_STENCIL_BUFFER_BIT;
		}

		// Are API flags set?
		if (flagsAPI)
		{
			// Set clear settings
			if (flags & Renderer::ClearFlag::COLOR)
			{
				glClearColor(color[0], color[1], color[2], color[3]);
			}
			if (flags & Renderer::ClearFlag::DEPTH)
			{
				glClearDepthf(z);
			}
			if (flags & Renderer::ClearFlag::STENCIL)
			{
				glClearStencil(static_cast<GLint>(stencil));
			}

			// Unlike OpenGL ES 2, when using Direct3D 10 & 11 the scissor rectangle(s) do not affect the clear operation
			// -> We have to compensate the OpenGL ES 2 behaviour in here

			// Disable OpenGL scissor test, in case it's not disabled, yet
			if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// Clear
			glClear(flagsAPI);

			// Restore the previously set OpenGL viewport
			if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glEnable(GL_SCISSOR_TEST);
			}
		}
	}

	bool OpenGLES2Renderer::beginScene()
	{
		// Not required when using OpenGL ES 2

		// Done
		return true;
	}

	void OpenGLES2Renderer::endScene()
	{
		// Not required when using OpenGL ES 2
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::draw(unsigned int startVertexLocation, unsigned int numberOfVertices)
	{
		// Is currently a vertex array set?
		if (nullptr != mVertexArray)
		{
			// Draw
			glDrawArrays(mOpenGLES2PrimitiveTopology, static_cast<GLint>(startVertexLocation), static_cast<GLsizei>(numberOfVertices));
		}
	}

	void OpenGLES2Renderer::drawInstanced(unsigned int, unsigned int, unsigned int)
	{
		// Error! OpenGL ES 2 has no instancing support!
	}

	void OpenGLES2Renderer::drawIndexed(unsigned int startIndexLocation, unsigned int numberOfIndices, unsigned int, unsigned int, unsigned int)
	{
		// Is currently an vertex array?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" equivalent, so, we can't support "baseVertexLocation" in here
				// OpenGL ES 2 has no "GL_EXT_draw_range_elements" equivalent, so, we can't support "minimumIndex" & "numberOfVertices" in here

				// Draw
				glDrawElements(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)));
			}
		}
	}

	void OpenGLES2Renderer::drawIndexedInstanced(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)
	{
		// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" equivalent, so, we can't support "baseVertexLocation" in here
		// Error! OpenGL ES 2 has no instancing support!
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::flush()
	{
		glFlush();
	}

	void OpenGLES2Renderer::finish()
	{
		glFinish();
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool OpenGLES2Renderer::isDebugEnabled()
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}

	void OpenGLES2Renderer::setDebugMarker(const wchar_t *)
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLES2Renderer::beginDebugEvent(const wchar_t *)
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLES2Renderer::endDebugEvent()
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Initialize the capabilities
	*/
	void OpenGLES2Renderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = 1;	// OpenGL ES 2 only supports a single viewport

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported, "GL_NV_draw_buffers" extension required)
		if (mContext->getExtensions().isGL_NV_draw_buffers())
		{
			glGetIntegerv(GL_MAX_DRAW_BUFFERS_NV, &openGLValue);
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = 1;
		}

		// Maximum texture dimension
		openGLValue = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
		mCapabilities.maximumTextureDimension = static_cast<unsigned int>(openGLValue);

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		if (mContext->getExtensions().isGL_EXT_texture_array())
		{
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS_EXT, &openGLValue);
			mCapabilities.maximumNumberOf2DTextureArraySlices = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOf2DTextureArraySlices = 0;
		}

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = false;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 0;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex, OpenGL ES 2 has no "GL_ARB_instanced_arrays" extension)
		mCapabilities.instancedArrays = false;

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, OpenGL ES 2 has no "GL_ARB_draw_instanced" extension)
		mCapabilities.drawInstanced = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = false;	// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" extension equivalent

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// OpenGL ES 2 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 0;	// OpenGL ES 2 has no support for geometry shaders

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
