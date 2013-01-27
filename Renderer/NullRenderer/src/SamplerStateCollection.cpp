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
#include "NullRenderer/SamplerStateCollection.h"
#include "NullRenderer/SamplerState.h"
#include "NullRenderer/NullRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	SamplerStateCollection::SamplerStateCollection(NullRenderer &nullRenderer, unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates) :
		Renderer::ISamplerStateCollection(nullRenderer),
		mNumberOfSamplerStates(numberOfSamplerStates),
		mSamplerStates((numberOfSamplerStates > 0) ? new Renderer::ISamplerState*[numberOfSamplerStates] : nullptr)
	{
		// Loop through all given textures
		Renderer::ISamplerState **currentSamplerState = mSamplerStates;
		Renderer::ISamplerState **samplerStateEnd	  = mSamplerStates + mNumberOfSamplerStates;
		for (; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++samplerStates)
		{
			// Get the current sampler state
			Renderer::ISamplerState *samplerState = *samplerStates;

			// Add a sampler state reference
			samplerState->addReference();
			*currentSamplerState = samplerState;

			// Security check: Is the given resource owned by this renderer?
			#ifndef NULLRENDERER_NO_RENDERERMATCHCHECK
				if (&nullRenderer != &samplerState->getRenderer())
				{
					// Output an error message and keep on going in order to keep a resonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("Null error: The given resource at index %d is owned by another renderer instance", currentSamplerState - mSamplerStates)
				}
			#endif
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	SamplerStateCollection::~SamplerStateCollection()
	{
		// Release the allocated memory
		if (nullptr != mSamplerStates)
		{
			// Loop through all given sampler states and release our references
			Renderer::ISamplerState **currentSamplerState = mSamplerStates;
			Renderer::ISamplerState **samplerStateEnd	  = mSamplerStates + mNumberOfSamplerStates;
			for (; currentSamplerState < samplerStateEnd; ++currentSamplerState)
			{
				(*currentSamplerState)->release();
			}

			// Release the allocated memory
			delete [] mSamplerStates;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
