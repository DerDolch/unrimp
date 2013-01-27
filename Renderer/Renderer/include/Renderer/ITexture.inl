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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Calculate the number of mipmaps
	*/
	inline unsigned int ITexture::getNumberOfMipmaps(unsigned int width, unsigned int height)
	{
		#define MAX(a, b) (a > b) ? a : b

		unsigned int numberOfMipmaps = 1;
		while ((width > 1) && (height > 1))
		{
			width  = MAX(width / 2,  static_cast<unsigned int>(1));
			height = MAX(height / 2, static_cast<unsigned int>(1));
			++numberOfMipmaps;
		}
		return numberOfMipmaps;

		#undef MAX
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Destructor
	*/
	inline ITexture::~ITexture()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	inline ITexture::ITexture(ResourceType::Enum resourceType, IRenderer &renderer) :
		IResource(resourceType, renderer)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	inline ITexture::ITexture(const ITexture &source) :
		IResource(source)
	{
		// Not supported
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline ITexture &ITexture::operator =(const ITexture &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
