/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __FIRSTINSTANCING_H__
#define __FIRSTINSTANCING_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    First instancing example
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - Index buffer object (IBO)
*    - Vertex shader (VS) and fragment shader (FS)
*    - Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
*    - Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
*/
class FirstInstancing : public IApplicationRenderer
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"
	*/
	explicit FirstInstancing(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstInstancing();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	// Left side (green): Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
	Renderer::IProgramPtr	  mProgramInstancedArrays;		/**< Program, can be a null pointer */
	Renderer::IVertexArrayPtr mVertexArrayInstancedArrays;	/**< Vertex array object (VAO), can be a null pointer */
	// Right side (blue): Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
	Renderer::IProgramPtr	  mProgramDrawInstanced;		/**< Program, can be a null pointer */
	Renderer::IVertexArrayPtr mVertexArrayDrawInstanced;	/**< Vertex array object (VAO), can be a null pointer */


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __FIRSTINSTANCING_H__
