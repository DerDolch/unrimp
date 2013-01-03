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
#include "Direct3D9Renderer/VertexShaderHlsl.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/ShaderLanguageHlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	VertexShaderHlsl::VertexShaderHlsl(Direct3D9Renderer &direct3D9Renderer, const char *sourceCode) :
		VertexShader(direct3D9Renderer),
		mDirect3DVertexShader9(nullptr),
		mD3DXConstantTable(nullptr)
	{
		// Create the Direct3D 9 buffer object for the vertex shader
		ID3DXBuffer *d3dXBuffer = ShaderLanguageHlsl::loadShader("vs_2_0", sourceCode, nullptr, &mD3DXConstantTable);
		if (nullptr != d3dXBuffer)
		{
			// Create the Direct3D 9 vertex shader
			direct3D9Renderer.getDirect3DDevice9()->CreateVertexShader(static_cast<DWORD*>(d3dXBuffer->GetBufferPointer()), &mDirect3DVertexShader9);

			// Release the Direct3D 9 shader buffer object
			d3dXBuffer->Release();
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	VertexShaderHlsl::~VertexShaderHlsl()
	{
		// Release the Direct3D 9 constant table
		if (nullptr != mD3DXConstantTable)
		{
			mD3DXConstantTable->Release();
		}

		// Release the Direct3D 9 vertex shader
		if (nullptr != mDirect3DVertexShader9)
		{
			mDirect3DVertexShader9->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexShaderHlsl::setDebugName(const char *)
	{
		// "IDirect3DVertexShader9" and "ID3DXConstantTable" are not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *VertexShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
