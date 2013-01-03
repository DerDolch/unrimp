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
#include "Renderer/PlatformTypes.h"	// For "RENDERER_OUTPUT_DEBUG_STRING()" and "RENDERER_OUTPUT_DEBUG_PRINTF()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Default constructor
	*/
	inline Statistics::Statistics() :
		currentNumberOfPrograms(0),
		numberOfCreatedPrograms(0),
		currentNumberOfVertexArrays(0),
		numberOfCreatedVertexArrays(0),
		// IRenderTarget
		currentNumberOfSwapChains(0),
		numberOfCreatedSwapChains(0),
		currentNumberOfFramebuffers(0),
		numberOfCreatedFramebuffers(0),
		// IBuffer
		currentNumberOfIndexBuffers(0),
		numberOfCreatedIndexBuffers(0),
		currentNumberOfVertexBuffers(0),
		numberOfCreatedVertexBuffers(0),
		currentNumberOfUniformBuffers(0),
		numberOfCreatedUniformBuffers(0),
		// ITexture
		currentNumberOfTextureBuffers(0),
		numberOfCreatedTextureBuffers(0),
		currentNumberOfTexture2Ds(0),
		numberOfCreatedTexture2Ds(0),
		currentNumberOfTexture2DArrays(0),
		numberOfCreatedTexture2DArrays(0),
		// IState
		currentNumberOfRasterizerStates(0),
		numberOfCreatedRasterizerStates(0),
		currentNumberOfDepthStencilStates(0),
		numberOfCreatedDepthStencilStates(0),
		currentNumberOfBlendStates(0),
		numberOfCreatedBlendStates(0),
		currentNumberOfSamplerStates(0),
		numberOfCreatedSamplerStates(0),
		// IShader
		currentNumberOfVertexShaders(0),
		numberOfCreatedVertexShaders(0),
		currentNumberOfTessellationControlShaders(0),
		numberOfCreatedTessellationControlShaders(0),
		currentNumberOfTessellationEvaluationShaders(0),
		numberOfCreatedTessellationEvaluationShaders(0),
		currentNumberOfGeometryShaders(0),
		numberOfCreatedGeometryShaders(0),
		currentNumberOfFragmentShaders(0),
		numberOfCreatedFragmentShaders(0),
		// ICollection
		currentNumberOfTextureCollections(0),
		numberOfCreatedTextureCollections(0),
		currentNumberOfSamplerStateCollections(0),
		numberOfCreatedSamplerStateCollections(0)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	inline Statistics::~Statistics()
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Return the number of current resource instances
	*/
	inline unsigned int Statistics::getNumberOfCurrentResources() const
	{
		// Calculate the current number of resource instances
		return	currentNumberOfPrograms +
				currentNumberOfVertexArrays +
				// IRenderTarget
				currentNumberOfSwapChains +
				currentNumberOfFramebuffers +
				// IBuffer
				currentNumberOfIndexBuffers +
				currentNumberOfVertexBuffers +
				currentNumberOfUniformBuffers +
				// ITexture
				currentNumberOfTextureBuffers +
				currentNumberOfTexture2Ds +
				currentNumberOfTexture2DArrays +
				// IState
				currentNumberOfRasterizerStates +
				currentNumberOfDepthStencilStates +
				currentNumberOfBlendStates +
				currentNumberOfSamplerStates +
				// IShader
				currentNumberOfVertexShaders +
				currentNumberOfTessellationControlShaders +
				currentNumberOfTessellationEvaluationShaders +
				currentNumberOfGeometryShaders +
				currentNumberOfFragmentShaders +
				// ICollection
				currentNumberOfTextureCollections +
				currentNumberOfSamplerStateCollections;
	}

	/**
	*  @brief
	*    Use debug output to show the current number of resource instances
	*/
	inline void Statistics::debugOutputCurrentResouces() const
	{
		// Start
		RENDERER_OUTPUT_DEBUG_STRING("** Number of current renderer resource instances **\n")

		// Misc
		RENDERER_OUTPUT_DEBUG_PRINTF("Programs: %d\n", currentNumberOfPrograms)
		RENDERER_OUTPUT_DEBUG_PRINTF("Vertex arrays: %d\n", currentNumberOfVertexArrays)

		// IRenderTarget
		RENDERER_OUTPUT_DEBUG_PRINTF("Swap chains: %d\n", currentNumberOfSwapChains)
		RENDERER_OUTPUT_DEBUG_PRINTF("Framebuffers: %d\n", currentNumberOfFramebuffers)

		// IBuffer
		RENDERER_OUTPUT_DEBUG_PRINTF("Index buffers: %d\n", currentNumberOfIndexBuffers)
		RENDERER_OUTPUT_DEBUG_PRINTF("Vertex buffers: %d\n", currentNumberOfVertexBuffers)
		RENDERER_OUTPUT_DEBUG_PRINTF("Uniform buffers: %d\n", currentNumberOfUniformBuffers)

		// ITexture
		RENDERER_OUTPUT_DEBUG_PRINTF("Texture buffers: %d\n", currentNumberOfTextureBuffers)
		RENDERER_OUTPUT_DEBUG_PRINTF("2D textures: %d\n", currentNumberOfTexture2Ds)
		RENDERER_OUTPUT_DEBUG_PRINTF("2D texture arrays: %d\n", currentNumberOfTexture2DArrays)

		// IState
		RENDERER_OUTPUT_DEBUG_PRINTF("Rasterizer states: %d\n", currentNumberOfRasterizerStates)
		RENDERER_OUTPUT_DEBUG_PRINTF("Depth stencil states: %d\n", currentNumberOfDepthStencilStates)
		RENDERER_OUTPUT_DEBUG_PRINTF("Blend states: %d\n", currentNumberOfBlendStates)
		RENDERER_OUTPUT_DEBUG_PRINTF("Sampler states: %d\n", currentNumberOfSamplerStates)

		// IShader
		RENDERER_OUTPUT_DEBUG_PRINTF("Vertex shaders: %d\n", currentNumberOfVertexShaders)
		RENDERER_OUTPUT_DEBUG_PRINTF("Tessellation control shaders: %d\n", currentNumberOfTessellationControlShaders)
		RENDERER_OUTPUT_DEBUG_PRINTF("Tessellation evaluation shaders: %d\n", currentNumberOfTessellationEvaluationShaders)
		RENDERER_OUTPUT_DEBUG_PRINTF("Geometry shaders: %d\n", currentNumberOfGeometryShaders)
		RENDERER_OUTPUT_DEBUG_PRINTF("Fragment shaders: %d\n", currentNumberOfFragmentShaders)

		// ICollection
		RENDERER_OUTPUT_DEBUG_PRINTF("Texture collections: %d\n", currentNumberOfTextureCollections)
		RENDERER_OUTPUT_DEBUG_PRINTF("Sampler state collections: %d\n", currentNumberOfSamplerStateCollections)

		// End
		RENDERER_OUTPUT_DEBUG_STRING("***************************************************\n")
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Copy constructor
	*/
	inline Statistics::Statistics(const Statistics &) :
		currentNumberOfPrograms(0),
		numberOfCreatedPrograms(0),
		currentNumberOfVertexArrays(0),
		numberOfCreatedVertexArrays(0),
		// IRenderTarget
		currentNumberOfSwapChains(0),
		numberOfCreatedSwapChains(0),
		currentNumberOfFramebuffers(0),
		numberOfCreatedFramebuffers(0),
		// IBuffer
		currentNumberOfIndexBuffers(0),
		numberOfCreatedIndexBuffers(0),
		currentNumberOfVertexBuffers(0),
		numberOfCreatedVertexBuffers(0),
		currentNumberOfUniformBuffers(0),
		numberOfCreatedUniformBuffers(0),
		// ITexture
		currentNumberOfTextureBuffers(0),
		numberOfCreatedTextureBuffers(0),
		currentNumberOfTexture2Ds(0),
		numberOfCreatedTexture2Ds(0),
		currentNumberOfTexture2DArrays(0),
		numberOfCreatedTexture2DArrays(0),
		// IState
		currentNumberOfRasterizerStates(0),
		numberOfCreatedRasterizerStates(0),
		currentNumberOfDepthStencilStates(0),
		numberOfCreatedDepthStencilStates(0),
		currentNumberOfBlendStates(0),
		numberOfCreatedBlendStates(0),
		currentNumberOfSamplerStates(0),
		numberOfCreatedSamplerStates(0),
		// IShader
		currentNumberOfVertexShaders(0),
		numberOfCreatedVertexShaders(0),
		currentNumberOfTessellationControlShaders(0),
		numberOfCreatedTessellationControlShaders(0),
		currentNumberOfTessellationEvaluationShaders(0),
		numberOfCreatedTessellationEvaluationShaders(0),
		currentNumberOfGeometryShaders(0),
		numberOfCreatedGeometryShaders(0),
		currentNumberOfFragmentShaders(0),
		numberOfCreatedFragmentShaders(0),
		// ICollection
		currentNumberOfTextureCollections(0),
		numberOfCreatedTextureCollections(0),
		currentNumberOfSamplerStateCollections(0),
		numberOfCreatedSamplerStateCollections(0)
	{
		// Not supported
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline Statistics &Statistics::operator =(const Statistics &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
