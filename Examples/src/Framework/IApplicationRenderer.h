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
#ifndef __IAPPLICATION_RENDERER_H__
#define __IAPPLICATION_RENDERER_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplication.h"

#include <Renderer/Renderer.h>


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef RENDERER_ONLY_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGL
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGLES2
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D9
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D10
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D11
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
#endif


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Renderer application interface
*/
class IApplicationRenderer : public IApplication
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IApplicationRenderer();

	/**
	*  @brief
	*    Return the renderer instance
	*
	*  @remarks
	*    The renderer instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
	*/
	inline Renderer::IRenderer *getRenderer() const;


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onResize() override;
	virtual void onToggleFullscreenState() override;
	virtual void onDrawRequest() override;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	*/
	explicit IApplicationRenderer(const char *rendererName);


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Create a renderer instance
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer nothing happens.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	*
	*  @return
	*    The created renderer instance, null pointer on error
	*/
	Renderer::IRenderer *createRendererInstance(const char *rendererName);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	char					mRendererName[64];		/**< Case sensitive ASCII name of the renderer to instance */
	void				   *mRendererSharedLibrary;	/**< Renderer shared library, can be a null pointer */
	Renderer::IRendererPtr  mRenderer;				/**< Renderer instance, can be a null pointer */


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "IApplicationRenderer.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __IAPPLICATION_RENDERER_H__
