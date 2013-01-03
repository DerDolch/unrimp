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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERTOOLKIT_RENDERERTOOLKITIMPL_H__
#define __RENDERERTOOLKIT_RENDERERTOOLKITIMPL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/IRendererToolkit.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IProgram;
	class IBlendState;
	class IVertexArray;
	class ISamplerState;
	class IUniformBuffer;
}
namespace RendererToolkit
{
	class FreeTypeContext;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderer toolkit class implementation
	*/
	class RendererToolkitImpl : public IRendererToolkit
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Renderer instance to use
		*
		*  @note
		*    - The renderer toolkit keeps a reference to the provided renderer instance
		*/
		explicit RendererToolkitImpl(Renderer::IRenderer &renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RendererToolkitImpl();

		/**
		*  @brief
		*    Return the FreeType context instance
		*
		*  @return
		*    The FreeType context instance, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - The FreeType context instance is created by using lazy evaluation, meaning that the instance is created the first time it's required
		*/
		FreeTypeContext &getFreeTypeContext();

		/**
		*  @brief
		*    Return the font program
		*
		*  @return
		*    The font program, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font program is created by using lazy evaluation, meaning that the font program is created the first time it's required
		*/
		Renderer::IProgram *getFontProgram();

		/**
		*  @brief
		*    Return the font vertex shader uniform buffer
		*
		*  @return
		*    The font vertex shader uniform buffer, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font vertex shader uniform buffer is created by using lazy evaluation, meaning that the font vertex shader uniform buffer is created the first time it's required
		*/
		Renderer::IUniformBuffer *getFontVertexShaderUniformBuffer();

		/**
		*  @brief
		*    Return the font fragment shader uniform buffer
		*
		*  @return
		*    The font fragment shader uniform buffer, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font fragment shader uniform buffer is created by using lazy evaluation, meaning that the font fragment shader uniform buffer is created the first time it's required
		*/
		Renderer::IUniformBuffer *getFontFragmentShaderUniformBuffer();

		/**
		*  @brief
		*    Return the font vertex array
		*
		*  @return
		*    The font vertex array, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font vertex array is created by using lazy evaluation, meaning that the font vertex array is created the first time it's required
		*/
		Renderer::IVertexArray *getVertexArray();

		/**
		*  @brief
		*    Return the font sampler state
		*
		*  @return
		*    The font sampler state, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font sampler state is created by using lazy evaluation, meaning that the font sampler state is created the first time it's required
		*/
		Renderer::ISamplerState *getFontSamplerState();

		/**
		*  @brief
		*    Return the font blend state
		*
		*  @return
		*    The font blend state, do not release the returned instance unless you added an own reference to it, can be a null pointer in case on an error
		*
		*  @note
		*    - The font blend state is created by using lazy evaluation, meaning that the font blend state is created the first time it's required
		*/
		Renderer::IBlendState *getFontBlendState();


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IRendererToolkit methods ]
	//[-------------------------------------------------------]
	public:
		virtual IFont *createFontTexture(const char *filename, unsigned int size = 12, unsigned int resolution = 96) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit RendererToolkitImpl(const RendererToolkitImpl &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline RendererToolkitImpl &operator =(const RendererToolkitImpl &source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		FreeTypeContext			 *mFreeTypeContext;					/**< FreeType context instance (we keep a reference to it), can be a null pointer, do only access it by using "getFreeTypeContext()" */
		Renderer::IProgram		 *mFontProgram;						/**< Font program (we keep a reference to it), can be a null pointer, do only access it by using "getFontProgram()" */
		Renderer::IUniformBuffer *mFontVertexShaderUniformBuffer;	/**< Font vertex shader uniform buffer (we keep a reference to it), can be a null pointer, do only access it by using "getFontVertexShaderUniformBuffer()" */
		Renderer::IUniformBuffer *mFontFragmentShaderUniformBuffer;	/**< Font fragment shader uniform buffer (we keep a reference to it), can be a null pointer, do only access it by using "getFontFragmentShaderUniformBuffer()" */
		Renderer::IVertexArray	 *mFontVertexArray;					/**< Vertex array object (VAO, we keep a reference to it), can be a null pointer, do only access it by using "getFontVertexArray()" */
		Renderer::ISamplerState	 *mFontSamplerState;				/**< Font sampler state (we keep a reference to it), can be a null pointer, do only access it by using "getFontSamplerState()" */
		Renderer::IBlendState	 *mFontBlendState;					/**< Font blend state state (we keep a reference to it), can be a null pointer, do only access it by using "getFontBlendState()" */


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/RendererToolkitImpl.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_RENDERERTOOLKITIMPL_H__
