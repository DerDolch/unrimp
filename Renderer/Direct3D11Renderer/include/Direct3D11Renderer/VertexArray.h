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
#ifndef __DIRECT3D11RENDERER_VERTEXARRAY_H__
#define __DIRECT3D11RENDERER_VERTEXARRAY_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IVertexArray.h>


//[-------------------------------------------------------]
//[ Forward declaration                                   ]
//[-------------------------------------------------------]
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11DeviceContext;
namespace Renderer
{
	struct VertexArrayAttribute;
}
namespace Direct3D11Renderer
{
	class IndexBuffer;
	class VertexBuffer;
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 vertex array class
	*/
	class VertexArray : public Renderer::IVertexArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] d3dBlob
		*    The loaded and compiled vertex shader
		*  @param[in] numberOfAttributes
		*    Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		*  @param[in] attributes
		*    At least nNumberOfAttributes instances of vertex array attributes, can be a null pointer in case there are zero attributes
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArray(Direct3D11Renderer &direct3D11Renderer, ID3DBlob &d3dBlob, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArray();

		/**
		*  @brief
		*    Return the Direct3D 11 input layout
		*
		*  @return
		*    Direct3D 11 input layout instance, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11InputLayout *getD3D11InputLayout() const;

		/**
		*  @brief
		*    Set the Direct3D 11 vertex declaration and stream source
		*/
		void setDirect3DIASetInputLayoutAndStreamSource() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3D11DeviceContext	 *mD3D11DeviceContext;		/**< The Direct3D 11 device context instance (we keep a reference to it), null pointer on horrible error (so we don't check) */
		IndexBuffer			 *mIndexBuffer;				/**< Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer */
		ID3D11InputLayout	 *mD3D11InputLayout;		/**< Direct3D 11 input layout, can be a null pointer */
		// Direct3D 11 input slots
		unsigned int		  mNumberOfSlots;			/**< Number of used Direct3D 11 input slots (type "UINT" not used in here in order to keep the header slim) */
		ID3D11Buffer		**mD3D11Buffers;			/**< Direct3D 11 vertex buffers, if "mD3D11InputLayout" is no null pointer this is no null pointer as well */
		unsigned int		 *mStrides;					/**< Strides in bytes, if "mD3D11Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim) */
		unsigned int		 *mOffsets;					/**< Offsets in bytes, if "mD3D11Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim) */
		// For proper vertex buffer reference counter behaviour
		unsigned int		  mNumberOfVertexBuffers;	/**< Number of vertex buffers */
		VertexBuffer		**mVertexBuffers;			/**< Vertex buffers (we keep a reference to it) used by this vertex array, can be a null pointer */


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/VertexArray.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_VERTEXARRAY_H__
