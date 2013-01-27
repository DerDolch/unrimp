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
//[ Shader start                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_CG
if (0 == strcmp(shaderLanguage->getShaderLanguageName(), "Cg"))
{


//[-------------------------------------------------------]
//[ Define helper macro                                   ]
//[-------------------------------------------------------]
#define STRINGIFY(ME) #ME


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode = STRINGIFY(
// Attribute input/output
struct VS_OUTPUT
{
	float4 Position : POSITION;		// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Normalized texture coordinate as output
};

// Programs
VS_OUTPUT main(float2 Position : POSITION)	// Clip space vertex position as input, left/bottom is (-1,-1) and right/top is (1,1)
{
	VS_OUTPUT output;

	// Pass through the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
	output.Position = float4(Position, 0.0f, 1.0f);

	// Calculate the texture coordinate
	// -> In OpenGL and OpenGL ES 2, the texture origin is left/bottom which maps well to clip space coordinates
	// -> Unlike OpenGL or OpenGL ES 2, in Direct3D 9 & 10 & 11 the texture origin is left/top which does not map well to clip space coordinates
	// -> We have to flip the y-axis to map the coordinate system to the Direct3D 9 & 10 & 11 texture coordinate system
	// -> In this example we do this within the fragment shader so have identical wobble across the different graphics APIs
	// -> (-1,-1) -> (0,0)
	// -> (1,1) -> (1,1)
	output.TexCoord = Position.xy * 0.5f + 0.5f;

	// Done
	return output;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code - Scene rendering         ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode_SceneRendering = STRINGIFY(
// Programs
float4 main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0) : COLOR0
{
	// Return the color green
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code - Post-processing         ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
// Get the proper version of the fragment shader
#if !defined(RENDERER_NO_OPENGL) || !defined(RENDERER_NO_OPENGLES2)
	if (0 == strncmp(shaderLanguage->getRenderer().getName(), "OpenGL", 6))
	{
		// OpenGL version
		static const char *fragmentShaderSourceCode_PostProcessing_OpenGL = STRINGIFY(
		// Uniforms
		uniform sampler2D DiffuseMap : register(s0);

		// Programs
		float4 main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0) : COLOR0
		{
			// Fetch the texel at the given texture coordinate and return it's color
			// -> Apply a simple wobble to the texture coordinate so we can see that post-processing is up and running
			return tex2D(DiffuseMap, float2(TexCoord.x + sin(TexCoord.x * 100.0f) * 0.01f, TexCoord.y + cos(TexCoord.y * 100.0f) * 0.01f));
		}
		);	// STRINGIFY
		fragmentShaderSourceCode_PostProcessing = fragmentShaderSourceCode_PostProcessing_OpenGL;
	}
else
#endif
#if !defined(RENDERER_NO_DIRECT3D9) || !defined(RENDERER_NO_DIRECT3D10) || !defined(RENDERER_NO_DIRECT3D11)
	if (0 == strncmp(shaderLanguage->getRenderer().getName(), "Direct3D", 7))
	{
		// Direct3D version
		static const char *fragmentShaderSourceCode_PostProcessing_Direct3D = STRINGIFY(
		// Uniforms
		uniform sampler2D DiffuseMap : register(s0);

		// Programs
		float4 main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0) : COLOR0
		{
			// Fetch the texel at the given texture coordinate and return it's color
			// -> Apply a simple wobble to the texture coordinate so we can see that post-processing is up and running
			// -> Unlike OpenGL or OpenGL ES 2, in Direct3D 9 & 10 & 11 the texture origin is left/top which does not map well to clip space coordinates
			// -> We have to flip the y-axis to map the coordinate system to the Direct3D 9 & 10 & 11 texture coordinate system
			// -> (-1,-1) -> (0,1)
			// -> (1,1) -> (1,0)
			return tex2D(DiffuseMap, float2(TexCoord.x + sin(TexCoord.x * 100.0f) * 0.01f, 1.0f - TexCoord.y - cos(TexCoord.y * 100.0f) * 0.01f));
		}
		);	// STRINGIFY
		fragmentShaderSourceCode_PostProcessing = fragmentShaderSourceCode_PostProcessing_Direct3D;
	}
#endif
{
	// Error! (unsupported renderer)
	OUTPUT_DEBUG_STRING("Error: Unsupported renderer\n")
}


//[-------------------------------------------------------]
//[ Undefine helper macro                                 ]
//[-------------------------------------------------------]
#undef STRINGIFY


//[-------------------------------------------------------]
//[ Shader end                                            ]
//[-------------------------------------------------------]
}
else
#endif
