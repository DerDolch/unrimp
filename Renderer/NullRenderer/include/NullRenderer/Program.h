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
#ifndef __NULLRENDERER_PROGRAM_H__
#define __NULLRENDERER_PROGRAM_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IProgram.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Forward declarations                                  ]
	//[-------------------------------------------------------]
	class NullRenderer;
	class VertexShader;
	class GeometryShader;
	class FragmentShader;
	class TessellationControlShader;
	class TessellationEvaluationShader;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Null program class
	*/
	class Program : public Renderer::IProgram
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nullRenderer
		*    Owner null renderer instance
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] tessellationControlShader
		*    Tessellation control shader the program is using, can be a null pointer
		*  @param[in] tessellationEvaluationShader
		*    Tessellation evaluation shader the program is using, can be a null pointer
		*  @param[in] geometryShader
		*    Geometry shader the program is using, can be a null pointer
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		Program(NullRenderer &nullRenderer, VertexShader *vertexShader, TessellationControlShader *tessellationControlShader, TessellationEvaluationShader *tessellationEvaluationShader, GeometryShader *geometryShader, FragmentShader *fragmentShader);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Program();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	public:
		virtual Renderer::IVertexArray *createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer = nullptr) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __NULLRENDERER_PROGRAM_H__
