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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/ShaderLanguageGlsl.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/ProgramGlsl.h"
#include "OpenGLRenderer/ProgramGlslDsa.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/VertexShaderGlsl.h"
#include "OpenGLRenderer/UniformBufferGlslDsa.h"
#include "OpenGLRenderer/UniformBufferGlslBind.h"
#include "OpenGLRenderer/GeometryShaderGlsl.h"
#include "OpenGLRenderer/FragmentShaderGlsl.h"
#include "OpenGLRenderer/TessellationControlShaderGlsl.h"
#include "OpenGLRenderer/TessellationEvaluationShaderGlsl.h"
#include <iostream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Creates, loads and compiles a shader
	*/
	unsigned int ShaderLanguageGlsl::loadShader(unsigned int shaderType, const char *shaderSource)
	{
		// Create the shader object
		const GLuint openGLShader = glCreateShaderObjectARB(shaderType);

		// Load the shader source
		glShaderSourceARB(openGLShader, 1, &shaderSource, nullptr);

		// Compile the shader
		glCompileShaderARB(openGLShader);

		// Check the compile status
		GLint compiled = GL_FALSE;
		glGetObjectParameterivARB(openGLShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
		if (GL_TRUE == compiled)
		{
			std::cout<<"shader compiled: "<<shaderType<<"\n";
			// All went fine, return the shader
			return openGLShader;
		}
		else
		{
			// Error, failed to compile the shader!
			#ifdef RENDERER_OUTPUT_DEBUG
				std::cout<<"shader compile failed: "<<shaderType<<"\n";
				// Get the length of the information
				GLint informationLength = 0;
				glGetObjectParameterivARB(openGLShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					GLchar *informationLog = new GLchar[static_cast<unsigned int>(informationLength)];

					// Get the information
					glGetInfoLogARB(openGLShader, informationLength, nullptr, informationLog);

					// Ouput the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif

			// Destroy the shader
			// -> A value of 0 for shader will be silently ignored
			glDeleteObjectARB(openGLShader);

			// Error!
			return 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ShaderLanguageGlsl::ShaderLanguageGlsl(OpenGLRenderer &openGLRenderer) :
		ShaderLanguage(openGLRenderer)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	ShaderLanguageGlsl::~ShaderLanguageGlsl()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's vertex shader support
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_vertex_shader())
		{
			return new VertexShaderGlsl(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no vertex shader support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationControlShaderGlsl(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationEvaluationShaderGlsl(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, unsigned int numberOfOutputVertices, const char *, const char *, const char *)
	{
		// Check whether or not there's geometry shader support
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_geometry_shader4())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderGlsl(openGLRenderer, sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
		}
		else
		{
			// Error! There's no geometry shader support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's fragment shader support
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_fragment_shader())
		{
			return new FragmentShaderGlsl(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no fragment shader support!
			return nullptr;
		}
	}

	Renderer::IProgram *ShaderLanguageGlsl::createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();

		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader && tessellationControlShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation control shader language mismatch!
		}
		else if (nullptr != tessellationEvaluationShader && tessellationEvaluationShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation evaluation shader language mismatch!
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}

		// Is "GL_EXT_direct_state_access" there?
		else if (openGLRenderer.getContext().getExtensions().isGL_EXT_direct_state_access())
		{
			// Effective direct state access (DSA)
			return new ProgramGlslDsa(openGLRenderer, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<TessellationControlShaderGlsl*>(tessellationControlShader), static_cast<TessellationEvaluationShaderGlsl*>(tessellationEvaluationShader), static_cast<GeometryShaderGlsl*>(geometryShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}
		else
		{
			// Traditional bind version
			return new ProgramGlsl(openGLRenderer, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<TessellationControlShaderGlsl*>(tessellationControlShader), static_cast<TessellationEvaluationShaderGlsl*>(tessellationEvaluationShader), static_cast<GeometryShaderGlsl*>(geometryShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->release();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->release();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}

		// Error!
		return nullptr;
	}

	Renderer::IUniformBuffer *ShaderLanguageGlsl::createUniformBuffer(unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// "GL_ARB_uniform_buffer_object" required
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_uniform_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (openGLRenderer.getContext().getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new UniformBufferGlslDsa(openGLRenderer, numberOfBytes, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new UniformBufferGlslBind(openGLRenderer, numberOfBytes, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
