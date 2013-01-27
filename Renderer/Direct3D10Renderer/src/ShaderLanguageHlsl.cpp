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
#include "Direct3D10Renderer/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/ProgramHlsl.h"
#include "Direct3D10Renderer/UniformBuffer.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/VertexShaderHlsl.h"
#include "Direct3D10Renderer/GeometryShaderHlsl.h"
#include "Direct3D10Renderer/FragmentShaderHlsl.h"
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageHlsl::NAME = "HLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Creates, loads and compiles a shader
	*/
	ID3DBlob *ShaderLanguageHlsl::loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		#if defined(DEBUG) || defined(_DEBUG)
			// Set the "D3DCOMPILE_DEBUG" flag to embed debug information in the shaders.
			// Setting this flag improves the shader debugging experience, but still allows
			// the shaders to be optimized and to run exactly the way they will run in
			// the release configuration of this program.
			shaderFlags |= D3DCOMPILE_DEBUG;
		#endif
		ID3DBlob *d3dBlob = nullptr;
		ID3DBlob *errorBlob = nullptr;
		if (FAILED(D3DX10CompileFromMemory(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, entryPoint ? entryPoint : "main", shaderModel, shaderFlags, 0, nullptr, &d3dBlob, &errorBlob, nullptr)))
		{
			if (nullptr != errorBlob)
			{
				::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
				errorBlob->Release();
			}

			// Error!
			return nullptr;
		}
		if (nullptr != errorBlob)
		{
			errorBlob->Release();
		}

		// Done
		return d3dBlob;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ShaderLanguageHlsl::ShaderLanguageHlsl(Direct3D10Renderer &direct3D10Renderer) :
		ShaderLanguage(direct3D10Renderer)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	ShaderLanguageHlsl::~ShaderLanguageHlsl()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageHlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageHlsl::createVertexShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(getDirect3D10Renderer(), sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageHlsl::createTessellationControlShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 10 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageHlsl::createTessellationEvaluationShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 10 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageHlsl::createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, unsigned int, const char *, const char *, const char *)
	{
		// Ignore "gsInputPrimitiveTopology", it's directly set within HLSL
		// Ignore "gsOutputPrimitiveTopology", it's directly set within HLSL
		// Ignore "numberOfOutputVertices", it's directly set within HLSL

		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return new GeometryShaderHlsl(getDirect3D10Renderer(), sourceCode);
	}

	Renderer::IFragmentShader *ShaderLanguageHlsl::createFragmentShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(getDirect3D10Renderer(), sourceCode);
	}

	Renderer::IProgram *ShaderLanguageHlsl::createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader)
		{
			// Error! Direct3D 10 has no tessellation control shader support.
		}
		else if (nullptr != tessellationEvaluationShader)
		{
			// Error! Direct3D 10 has no tessellation evaluation shader support.
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			// Create the program
			return new ProgramHlsl(getDirect3D10Renderer(), static_cast<VertexShaderHlsl*>(vertexShader), static_cast<GeometryShaderHlsl*>(geometryShader), static_cast<FragmentShaderHlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
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

	Renderer::IUniformBuffer *ShaderLanguageHlsl::createUniformBuffer(unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new UniformBuffer(getDirect3D10Renderer(), numberOfBytes, data, bufferUsage);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
