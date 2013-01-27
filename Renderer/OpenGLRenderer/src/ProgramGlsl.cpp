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
#include "OpenGLRenderer/ProgramGlsl.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/IndexBuffer.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/VertexShaderGlsl.h"
#include "OpenGLRenderer/VertexArrayNoVao.h"
#include "OpenGLRenderer/VertexArrayVaoDsa.h"
#include "OpenGLRenderer/VertexArrayVaoBind.h"
#include "OpenGLRenderer/GeometryShaderGlsl.h"
#include "OpenGLRenderer/FragmentShaderGlsl.h"
#include "OpenGLRenderer/TessellationControlShaderGlsl.h"
#include "OpenGLRenderer/TessellationEvaluationShaderGlsl.h"


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
	ProgramGlsl::ProgramGlsl(OpenGLRenderer &openGLRenderer, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		Program(openGLRenderer, InternalResourceType::GLSL),
		mOpenGLProgram(glCreateProgramObjectARB())
	{
		// Attach the shaders to the program
		// -> We don't need to keep a reference to the shader, to add and release at once to ensure a nice behaviour
		if (nullptr != vertexShaderGlsl)
		{
			vertexShaderGlsl->addReference();
			glAttachObjectARB(mOpenGLProgram, vertexShaderGlsl->getOpenGLShader());
			vertexShaderGlsl->release();
		}
		if (nullptr != tessellationControlShaderGlsl)
		{
			tessellationControlShaderGlsl->addReference();
			glAttachObjectARB(mOpenGLProgram, tessellationControlShaderGlsl->getOpenGLShader());
			tessellationControlShaderGlsl->release();
		}
		if (nullptr != tessellationEvaluationShaderGlsl)
		{
			tessellationEvaluationShaderGlsl->addReference();
			glAttachObjectARB(mOpenGLProgram, tessellationEvaluationShaderGlsl->getOpenGLShader());
			tessellationEvaluationShaderGlsl->release();
		}
		if (nullptr != geometryShaderGlsl)
		{
			// Add a reference to the shader
			geometryShaderGlsl->addReference();

			// Attach the GLSL shader to the GLSL program
			glAttachObjectARB(mOpenGLProgram, geometryShaderGlsl->getOpenGLShader());

			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions

			// Set the OpenGL geometry shader input primitive topology
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_INPUT_TYPE_ARB, geometryShaderGlsl->getOpenGLGsInputPrimitiveTopology());

			// Set the OpenGL geometry shader output primitive topology
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_OUTPUT_TYPE_ARB, geometryShaderGlsl->getOpenGLGsOutputPrimitiveTopology());

			// Set the number of output vertices
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_VERTICES_OUT_ARB, static_cast<GLint>(geometryShaderGlsl->getNumberOfOutputVertices()));

			// Release the shader
			geometryShaderGlsl->release();
		}
		if (nullptr != fragmentShaderGlsl)
		{
			fragmentShaderGlsl->addReference();
			glAttachObjectARB(mOpenGLProgram, fragmentShaderGlsl->getOpenGLShader());
			fragmentShaderGlsl->release();
		}

		// Link the program
		glLinkProgramARB(mOpenGLProgram);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetObjectParameterivARB(mOpenGLProgram, GL_OBJECT_LINK_STATUS_ARB, &linked);
		if (GL_TRUE != linked)
		{
			// Error, program link failed!
			#ifdef RENDERER_OUTPUT_DEBUG
				// Get the length of the information (including a null termination)
				GLint informationLength = 0;
				glGetObjectParameterivARB(mOpenGLProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					char *informationLog = new char[static_cast<unsigned int>(informationLength)];

					// Get the information
					glGetInfoLogARB(mOpenGLProgram, informationLength, nullptr, informationLog);

					// Ouput the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ProgramGlsl::~ProgramGlsl()
	{
		// Destroy the OpenGL program
		// -> A value of 0 for program will be silently ignored
		glDeleteObjectARB(mOpenGLProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	Renderer::IVertexArray *ProgramGlsl::createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer)
	{
		// Get the extensions instance
		const Extensions &extensions = static_cast<OpenGLRenderer&>(getRenderer()).getContext().getExtensions();

		// Is "GL_ARB_vertex_array_object" there?
		if (extensions.isGL_ARB_vertex_array_object())
		{
			// Effective vertex array object (VAO)

			// Is "GL_EXT_direct_state_access" there?
			if (extensions.isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				return new VertexArrayVaoDsa(*this, numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
			}
			else
			{
				// Traditional bind version
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				return new VertexArrayVaoBind(*this, numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
			}
		}
		else
		{
			// Traditional version
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayNoVao(*this, numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
		}
	}

	int ProgramGlsl::getAttributeLocation(const char *attributeName)
	{
		return glGetAttribLocationARB(mOpenGLProgram, attributeName);
	}

	unsigned int ProgramGlsl::getUniformBlockIndex(const char *uniformBlockName, unsigned int)
	{
		// Explicit binding points ("layout(binding=0)" in GLSL shader) requires OpenGL 4.2, for backward compatibility, ask for the uniform block index
		return glGetUniformBlockIndex(mOpenGLProgram, uniformBlockName);
	}

	handle ProgramGlsl::getUniformHandle(const char *uniformName)
	{
		return static_cast<handle>(glGetUniformLocationARB(mOpenGLProgram, uniformName));
	}

	unsigned int ProgramGlsl::setTextureUnit(handle uniformHandle, unsigned int unit)
	{
		// OpenGL/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
		// -> Explicit binding points ("layout(binding=0)" in GLSL shader) requires OpenGL 4.2
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1iARB(static_cast<GLint>(uniformHandle), unit);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform1iARB(static_cast<GLint>(uniformHandle), unit);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform1iARB(static_cast<GLint>(uniformHandle), static_cast<GLint>(unit));
		#endif
		return unit;
	}

	void ProgramGlsl::setUniform1i(handle uniformHandle, int value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1iARB(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform1iARB(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform1iARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramGlsl::setUniform1f(handle uniformHandle, float value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1fARB(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform1fARB(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform1fARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramGlsl::setUniform2fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniform3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniform4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}

	void ProgramGlsl::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLint openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
