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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __DIRECT3D9RENDERER_SAMPLERSTATECOLLECTION_H__
#define __DIRECT3D9RENDERER_SAMPLERSTATECOLLECTION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ISamplerStateCollection.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ISamplerState;
}
namespace Direct3D9Renderer
{
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 9 sampler state collection class
	*/
	class SamplerStateCollection : public Renderer::ISamplerStateCollection
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*  @param[in] numberOfSamplerStates
		*    The number of sampler states
		*  @param[in] samplerStates
		*    The sampler states, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfSamplerStates" sampler states in the provided C-array of pointers
		*
		*  @note
		*    - The sampler state collection keeps a reference to the provided sampler state instances
		*/
		SamplerStateCollection(Direct3D9Renderer &direct3D9Renderer, unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SamplerStateCollection();

		/**
		*  @brief
		*    Return the number of sampler state instances
		*
		*  @return
		*    The number of sampler state instances
		*/
		inline unsigned int getNumberOfSamplerStates() const;

		/**
		*  @brief
		*    Return the sampler state instances
		*
		*  @return
		*    The sampler state instances, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::ISamplerState **getSamplerStates() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int			  mNumberOfSamplerStates;	/**< The number of sampler state instances */
		Renderer::ISamplerState **mSamplerStates;			/**< Sampler state instances (we keep a reference to it), can be a null pointer */


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/SamplerStateCollection.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D9RENDERER_SAMPLERSTATECOLLECTION_H__
