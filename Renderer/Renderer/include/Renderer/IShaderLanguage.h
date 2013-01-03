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
#ifndef __RENDERER_ISHADERLANGUAGE_H__
#define __RENDERER_ISHADERLANGUAGE_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/BufferTypes.h"
#include "Renderer/SmartRefCount.h"
#include "Renderer/GeometryShaderTypes.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Forward declarations                                  ]
	//[-------------------------------------------------------]
	class IBuffer;
	class IProgram;
	class IRenderer;
	class IVertexShader;
	class IUniformBuffer;
	class IGeometryShader;
	class IFragmentShader;
	class ITessellationControlShader;
	class ITessellationEvaluationShader;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract shader language interface
	*/
	class IShaderLanguage : public RefCount<IShaderLanguage>
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IShaderLanguage();

		/**
		*  @brief
		*    Return the owner renderer instance
		*
		*  @return
		*    The owner renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline IRenderer &getRenderer() const;

		/**
		*  @brief
		*    Create a program and assigns a vertex and fragment shader to it
		*
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram *createProgram(IVertexShader *vertexShader, IFragmentShader *fragmentShader);

		/**
		*  @brief
		*    Create a program and assigns a vertex, geometry and fragment shader to it
		*
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] geometryShader
		*    Geometry shader the program is using, can be a null pointer, geometry shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram *createProgram(IVertexShader *vertexShader, IGeometryShader *geometryShader, IFragmentShader *fragmentShader);

		/**
		*  @brief
		*    Create a program and assigns a vertex, tessellation control, tessellation evaluation and fragment shader to it
		*
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] tessellationControlShader
		*    Tessellation control shader the program is using, can be a null pointer, tessellation control shader and program language must match!
		*  @param[in] tessellationEvaluationShader
		*    Tessellation evaluation shader the program is using, can be a null pointer, tessellation evaluation shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram *createProgram(IVertexShader *vertexShader, ITessellationControlShader *tessellationControlShader, ITessellationEvaluationShader *tessellationEvaluationShader, IFragmentShader *fragmentShader);


	//[-------------------------------------------------------]
	//[ Public virtual IShaderLanguage methods                ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the name of the shader language
		*
		*  @return
		*    The ASCII name of the shader language (for example "GLSL", "HLSL" or "Cg"), never a null pointer
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		virtual const char *getShaderLanguageName() const = 0;

		/**
		*  @brief
		*    Create a vertex shader and sets the shader source code
		*
		*  @param[in] sourceCode
		*    Shader ASCII source code, if null pointer or empty string a null pointer will be returned
		*  @param[in] profile
		*    Shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually
		*    tries to use the best available profile that runs on most hardware (Examples: "glslv", "arbvp1", "vs_3_0")
		*  @param[in] arguments
		*    Optional shader compiler ASCII arguments, e.g. "version=150" when using Cg and a "glslv" profile, can be a null pointer or empty string
		*  @param[in] entry
		*    ASCII entry point, if null pointer or empty string, "main" is used as default
		*
		*  @return
		*    The created vertex shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    "profile" is not supported by each shader-API and is in general shader-API dependent. GLSL doesn't have such
		*    profiles, just something named "version" - one has to directly write into the shader. But even when this information
		*    is not used for compiling the GLSL shader, we highly recommend to provide GLSL version information in the form of e.g.
		*    "130" for OpenGL 3.0 shaders ("#version 130"). Within Cg, a basic vertex profile may be "arbvp1" and a basic
		*    fragment profile "arbfp1". Cg also provides GLSL profiles: "glslv" for vertex shader, "glslg" for geometry shader and
		*    "glslf" for fragment shader.
		*    Please note that the profile is just a hint, if necessary, the implementation is free to choose another profile.
		*    In general, be carefully when explicitly setting a profile - when using Cg, and one of the shaders, a GPU program
		*    is composed of, is a GLSL profile like "glslv", all other shaders must use GLSL profiles as well!
		*
		*   "entry" is not supported by each shader-API. GLSL doesn't have such an user defined entry point and the main
		*   function must always be "main". Cg supports entry points with names other than "main".
		*
		*   Look out! When working with shaders you have to be prepared that a shader may work on one system, but fails to even
		*   compile on another one. Sadly, even if there are e.g. official GLSL specifications, you can't be sure that every
		*   GPU driver is implementing them in detail. Here are some pitfalls which already produced some headaches...
		*
		*   When using GLSL, don't forget to provide the #version directive! Quote from
		*     "The OpenGL� Shading Language - Language Version: 3.30 - Document Revision: 6 - 11-Mar-2010" Page 14
		*       "Version 1.10 of the language does not require shaders to include this directive, and shaders that do not include
		*        a #version directive will be treated as targeting version 1.10."
		*   It looks like that AMD/ATI drivers ("AMD Catalyst� 11.3") are NOT using just version 1.10 if there's no #version directive, but a higher
		*   version... so don't trust your GPU driver when your GLSL code, using modern language features, also works for you without
		*   #version directive, because it may not on other systems! OpenGL version and GLSL version can be a bit confusing, so
		*   here's a version table:
		*     GLSL #version    OpenGL version    Some comments
		*     110              2.0
		*     120              2.1
		*     130              3.0               Precision qualifiers added
		*                                        "attribute" deprecated; linkage between a vertex shader and OpenGL for per-vertex data -> use "in" instead
		*                                        "varying"/"centroid varying" deprecated; linkage between a vertex shader and a fragment shader for interpolated data -> use "in"/"out" instead
		*     140              3.1
		*     150              3.2               Almost feature-identical to Direct3D Shader Model 4.0 (Direct3D version 10), geometry shader added
		*     330              3.3               Equivalent to Direct3D Shader Model 4.0 (Direct3D version 10)
		*     400              4.0               Tessellation control ("Hull"-Shader in Direct3D 11) and evaluation ("Domain"-Shader in Direct3D 11) shaders added
		*     410              4.1
		*     420              4.2               Equivalent to Direct3D Shader Model 5.0 (Direct3D version 11)
		*  #version must occur before any other statement in the program as stated within:
		*    "The OpenGL� Shading Language - Language Version: 3.30 - Document Revision: 6 - 11-Mar-2010" Page 15
		*      "The #version directive must occur in a shader before anything else, except for comments and white space."
		*  ... sadly, this time NVIDIA (driver: "266.58 WHQL") is not implementing the specification in detail and while on AMD/ATI drivers ("AMD Catalyst� 11.3")
		*  you get the error message "error(#105) #version must occur before any other statement in the program" when breaking specification,
		*  NVIDIA just accepts it without any error.
		*
		*  Enough on GLSL - now to Cg. Sadly, in general Cg is on AMD/ATI GPU's just poor due to the lack of modern profiles. When using Cg on none NVIDIA
		*  GPU's you have virtually no other change then using the GLSL profiles in order to write shaders using modern features. While the concept of Cg is
		*  fantastic, this lack of modern none NVIDIA profiles destroys many of Cg's advantages...
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::vertexShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual IVertexShader *createVertexShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;

		/**
		*  @brief
		*    Create a tessellation control shader ("hull shader" in Direct3D terminology) and sets the shader source code
		*
		*  @param[in] sourceCode
		*    Shader ASCII source code, if null pointer or empty string a null pointer will be returned
		*  @param[in] profile
		*    Shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually
		*    tries to use the best available profile that runs on most hardware (Example: "hs_5_0")
		*  @param[in] arguments
		*    Optional shader compiler ASCII arguments ("February 2012 version of Cg 3.1": No GLSL tessellation control shader support, at least one that's not using special NVIDIA-only extensions), can be a null pointer or empty string
		*  @param[in] entry
		*    ASCII entry point, if null pointer or empty string, "main" is used as default
		*
		*  @return
		*    The created tessellation control shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual ITessellationControlShader *createTessellationControlShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;

		/**
		*  @brief
		*    Create a tessellation evaluation shader ("domain shader" in Direct3D terminology) and sets the shader source code
		*
		*  @param[in] sourceCode
		*    Shader ASCII source code, if null pointer or empty string a null pointer will be returned
		*  @param[in] profile
		*    Shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually
		*    tries to use the best available profile that runs on most hardware (Example: "ds_5_0")
		*  @param[in] arguments
		*    Optional shader compiler ASCII arguments ("February 2012 version of Cg 3.1": No GLSL tessellation evaluation shader support, at least one that's not using special NVIDIA-only extensions), can be a null pointer or empty string
		*  @param[in] entry
		*    ASCII entry point, if null pointer or empty string, "main" is used as default
		*
		*  @return
		*    The created tessellation evaluation shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual ITessellationEvaluationShader *createTessellationEvaluationShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;

		/**
		*  @brief
		*    Create a geometry shader and sets the shader source code
		*
		*  @param[in] sourceCode
		*    Geometry shader ASCII source code, if null pointer or empty string a null pointer will be returned
		*  @param[in] gsInputPrimitiveTopology
		*    Geometry shader input primitive topology
		*  @param[in] gsOutputPrimitiveTopology
		*    Geometry shader output primitive topology
		*  @param[in] numberOfOutputVertices
		*    Number of output vertices, maximum is "Renderer::Capabilities::maximumNumberOfGsOutputVertices"
		*  @param[in] profile
		*    Geometry shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually
		*    tries to use the best available profile that runs on most hardware (Examples: "glslg", "gs_4_0")
		*  @param[in] arguments
		*    Optional shader compiler ASCII arguments, e.g. "version=150" when using Cg and a "glslg" profile, can be a null pointer or empty string
		*  @param[in] entry
		*    ASCII entry point, if null pointer or empty string, "main" is used as default
		*
		*  @return
		*    The created geometry shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfGsOutputVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*    - Please note that not each internal implementation may actually need information like "gsInputPrimitiveTopology", but it's
		*      highly recommended to provide this information anyway to be able to switch the internal implementation (e.g. using
		*      OpenGL instead of Direct3D and/or Cg instead of HLSL/GLSL)
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual IGeometryShader *createGeometryShader(const char *sourceCode, GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, unsigned int numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;

		/**
		*  @brief
		*    Create a fragment shader and sets the shader source code
		*
		*  @param[in] sourceCode
		*    Shader ASCII source code, if null pointer or empty string a null pointer will be returned
		*  @param[in] profile
		*    Shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually
		*    tries to use the best available profile that runs on most hardware (Examples: "glslf", "arbfp1", "ps_3_0")
		*  @param[in] arguments
		*    Optional shader compiler ASCII arguments, e.g. "version=150" when using Cg and a "glslf" profile, can be a null pointer or empty string
		*  @param[in] entry
		*    ASCII entry point, if null pointer or empty string, "main" is used as default
		*
		*  @return
		*    The created fragment shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::fragmentShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual IFragmentShader *createFragmentShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;

		/**
		*  @brief
		*    Create a program and assigns a vertex, tessellation control, tessellation evaluation, geometry and fragment shader to it
		*
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] tessellationControlShader
		*    Tessellation control shader the program is using, can be a null pointer, tessellation control shader and program language must match!
		*  @param[in] tessellationEvaluationShader
		*    Tessellation evaluation shader the program is using, can be a null pointer, tessellation evaluation shader and program language must match!
		*  @param[in] geometryShader
		*    Geometry shader the program is using, can be a null pointer, geometry shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required,
		*      so it's safe to directly hand over a fresh created resource without releasing it manually
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*/
		virtual IProgram *createProgram(IVertexShader *vertexShader, ITessellationControlShader *tessellationControlShader, ITessellationEvaluationShader *tessellationEvaluationShader, IGeometryShader *geometryShader, IFragmentShader *fragmentShader) = 0;

		/**
		*  @brief
		*    Create an uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the uniform buffer, must be valid
		*  @param[in] data
		*    Uniform buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created UBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual IUniformBuffer *createUniformBuffer(unsigned int numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IShaderLanguage(IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IShaderLanguage(const IShaderLanguage &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IShaderLanguage &operator =(const IShaderLanguage &source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRenderer *mRenderer;	/**< The owner renderer instance, always valid */


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IShaderLanguage> IShaderLanguagePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/IShaderLanguage.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_ISHADERLANGUAGE_H__
