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
#ifndef __RENDERER_GEOMETRYSHADER_TYPES_H__
#define __RENDERER_GEOMETRYSHADER_TYPES_H__


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Geometry shader (GS) input primitive topology
	*
	*  @note
	*    - These constants directly map to OpenGL constants, do not change them
	*/
	struct GsInputPrimitiveTopology
	{
		enum Enum
		{
			POINTS			    = 0x0000,	/**< List of point primitives */
			LINES			    = 0x0001,	/**< List of line or line strip primitives */
			LINES_ADJACENCY	    = 0x000A,	/**< List of line with adjacency or line strip with adjacency primitives */
			TRIANGLES		    = 0x0004,	/**< List of triangle or triangle strip primitives */
			TRIANGLES_ADJACENCY = 0x000C	/**< List of triangle with adjacency or triangle strip with adjacency primitives */
		};
	};

	/**
	*  @brief
	*    Geometry shader (GS) primitive topology
	*
	*  @note
	*    - These constants directly map to OpenGL constants, do not change them
	*/
	struct GsOutputPrimitiveTopology
	{
		enum Enum
		{
			POINTS			=  0x0000,	/**< A list of of point primitives */
			LINES			=  0x0001,	/**< A list of line primitives */
			TRIANGLES_STRIP	=  0x0005	/**< A triangle strip primitives */
		};
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_GEOMETRYSHADER_TYPES_H__
