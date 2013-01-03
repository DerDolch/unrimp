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
#include "OpenGLRenderer/BlendState.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	BlendState::BlendState(OpenGLRenderer &openGLRenderer, const Renderer::BlendState &blendState) :
		IBlendState(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mBlendState(blendState)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	BlendState::~BlendState()
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Set the OpenGL blend states
	*/
	void BlendState::setOpenGLBlendStates() const
	{
		if (mBlendState.renderTarget[0].blendEnable)
		{
			glEnable(GL_BLEND);

			// TODO(co) Add more blend state options: Due to time limitations for now only fixed build in alpha blend setup in order to see a change
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		// TODO(co) Map the rest of the blend states
		// GL_EXT_blend_func_separate
		// (GL_EXT_blend_equation_separate)
		// GL_EXT_blend_color
		// GL_EXT_blend_minmax
		// GL_EXT_blend_subtract
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
