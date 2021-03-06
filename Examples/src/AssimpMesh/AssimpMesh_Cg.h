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


// By default, OpenGL GLSL profiles are used by the renderer implementation to have access to more modern features
// -> Sadly, it appears that there are some issues with vertex attributes when using Cg for OpenGL with GLSL on AMD GPU's (tested on "AMD Radeon HD6970M")
// -> In this example, the result is a black mesh drawn on the gray background
// -> Used Cg version: "Cg Toolkit 3.1 - April 2012 (3.1.0013)" (this issue existed also in previous versions, since forever)
// -> On the other hand, the "arbvp1" & "arbfp1" Cg profiles have no such issues and are quite stable, so, use those in here
if (0 == strcmp(renderer->getName(), "OpenGL"))
{
	vertexShaderProfile   = "arbvp1";	// GL_ARB_vertex_program
	fragmentShaderProfile = "arbfp1";	// GL_ARB_fragment_program
}


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
	float2 TexCoord : TEXCOORD0;	// Texture coordinate
	float3 Tangent  : TEXCOORD1;	// Tangent space to view space, x-axis
	float3 Binormal : TEXCOORD2;	// Tangent space to view space, y-axis
	float3 Normal   : TEXCOORD3;	// Tangent space to view space, z-axis
};

// Uniforms
uniform float4x4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix
uniform float3x3 ObjectSpaceToViewSpaceMatrix;	// Object space to view space matrix

// Programs
// -> Do not put those vertex attributes in a structure, else it will not be possible
//    to address them by name under OpenGL in the same way as when using GLSL
VS_OUTPUT main(float3 Position : POSITION,	// Object space vertex position
			   float2 TexCoord : TEXCOORD0,	// Texture coordinate
			   float3 Tangent  : TEXCOORD1,	// Object space to tangent space, x-axis
			   float3 Binormal : TEXCOORD2,	// Object space to tangent space, y-axis
			   float3 Normal   : NORMAL)	// Object space to tangent space, z-axis
{
	VS_OUTPUT output;

	// Calculate the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
	output.Position = mul(ObjectSpaceToClipSpaceMatrix, float4(Position, 1.0f));

	// Pass through the texture coordinate
	output.TexCoord = TexCoord;

	// Calculate the tangent space to view space tangent, binormal and normal
	output.Binormal = mul(ObjectSpaceToViewSpaceMatrix, Binormal);
	output.Tangent  = mul(ObjectSpaceToViewSpaceMatrix, Tangent);
	output.Normal   = mul(ObjectSpaceToViewSpaceMatrix, Normal);

	// Done
	return output;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
// "pixel shader" in Direct3D terminology
fragmentShaderSourceCode = STRINGIFY(
// Attribute input/output
struct VS_OUTPUT
{
	float4 Position : POSITION;		// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Texture coordinate
	float3 Tangent  : TEXCOORD1;	// Tangent space to view space, x-axis
	float3 Binormal : TEXCOORD2;	// Tangent space to view space, y-axis
	float3 Normal   : TEXCOORD3;	// Tangent space to view space, z-axis
};

// Uniforms
uniform sampler2D DiffuseMap  : register(s0);
uniform sampler2D EmissiveMap : register(s1);
uniform sampler2D NormalMap   : register(s2);	// Tangent space normal map
uniform sampler2D SpecularMap : register(s3);

// Programs
float4 main(VS_OUTPUT input) : COLOR0
{
	// Build in variables
	float3 ViewSpaceLightDirection = normalize(float3(0.5f, 0.5f, 1.0f));	// View space light direction
	float3 ViewSpaceViewVector     = float3(0.0f, 0.0f, 1.0f);				// In view space, we always look along the positive z-axis

	// Get the per fragment normal [0..1] by using a tangent space normal map
	float3 normal = tex2D(NormalMap, input.TexCoord).rgb;

	// Transform the normal from [0..1] to [-1..1]
	normal = (normal - 0.5f) * 2.0f;

	// Transform the tangent space normal into view space
	normal = normalize(normal.x * input.Tangent + normal.y * input.Binormal + normal.z * input.Normal);

	// Perform standard Blinn-Phong diffuse and specular lighting

	// Calculate the diffuse lighting
	float diffuseLight = max(dot(normal, ViewSpaceLightDirection), 0.0f);

	// Calculate the specular lighting
	float3 viewSpaceHalfVector = normalize(ViewSpaceLightDirection + ViewSpaceViewVector);
	float specularLight = (diffuseLight > 0.0f) ? pow(max(dot(normal, viewSpaceHalfVector), 0.0f), 128.0f) : 0.0f;

	// Calculate the fragment color
	float4 color = diffuseLight * tex2D(DiffuseMap, input.TexCoord);		// Diffuse term
	color.rgb += specularLight * tex2D(SpecularMap, input.TexCoord).rgb;	// Specular term
	color.rgb += tex2D(EmissiveMap, input.TexCoord).rgb;					// Emissive term

	// Done
	return min(color, float4(1.0f, 1.0f, 1.0f, 1.0f));
}
);	// STRINGIFY


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
