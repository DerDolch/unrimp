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
#include "Direct3D10Renderer/FragmentShaderCg.h"
#include "Direct3D10Renderer/CgRuntimeLinking.h"
#include "Direct3D10Renderer/ShaderLanguageCg.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_STRING()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	FragmentShaderCg::FragmentShaderCg(Direct3D10Renderer &direct3D10Renderer, _CGcontext &cgContext, const char *sourceCode) :
		FragmentShader(direct3D10Renderer),
		mCgProgram(nullptr)
	{
		// Get the profile from a user given string
		CGprofile cgProfile = cgGetProfile("ps_4_0");

		// On unknown or invalid profile, choose a fallback profile
		if (CG_PROFILE_UNKNOWN == cgProfile || !cgGetProfileProperty(cgProfile, CG_IS_FRAGMENT_PROFILE))
		{
			cgProfile = cgD3D10GetLatestPixelProfile();
		}

		// Create the Cg program
		if (CG_PROFILE_UNKNOWN != cgProfile)
		{
			mCgProgram = cgCreateProgram(&cgContext, CG_SOURCE, sourceCode, cgProfile, "main", nullptr);
			if (nullptr != mCgProgram)
			{
				// Load the Cg combined program
				// -> The Cg documentation mentions "D3D_OK" (Direct3D 9), I assume it's "S_OK"
				// -> See "Direct3D 10 Return Codes" at http://msdn.microsoft.com/en-us/library/windows/desktop/bb205278%28v=vs.85%29.aspx
				// TODO(co) I am not sure what's going on, as soon as "cgD3D10LoadProgram()" is used a memory leak appears when terminating the application
				//        -> Used Cg version: "Cg Toolkit 3.1 - April 2012 (3.1.0013)"
				if (S_OK != cgD3D10LoadProgram(mCgProgram, 0))
				{
					// Error!
					// TODO(co) More error details
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 Cg error: Failed to load the Cg program\n")
				}
			}
			else
			{
				// Error!
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 Cg error: Failed to create the Cg program\n")
			}
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	FragmentShaderCg::~FragmentShaderCg()
	{
		// Destroy the Cg program
		if (nullptr != mCgProgram)
		{
			cgDestroyProgram(mCgProgram);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *FragmentShaderCg::getShaderLanguageName() const
	{
		return ShaderLanguageCg::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
