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
#include "OpenGLRenderer/VertexBufferDsa.h"
#include "OpenGLRenderer/Extensions.h"


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
	VertexBufferDsa::VertexBufferDsa(OpenGLRenderer &openGLRenderer, unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage) :
		VertexBuffer(openGLRenderer)
	{
		// Upload the data
		// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
		glNamedBufferDataEXT(mOpenGLArrayBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, bufferUsage);
	}

	/**
	*  @brief
	*    Destructor
	*/
	VertexBufferDsa::~VertexBufferDsa()
	{
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
