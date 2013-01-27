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
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Destructor
	*/
	inline IFontGlyph::~IFontGlyph()
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Return the width and height (both in pixel) of the glyph
	*/
	inline void IFontGlyph::getWidthAndHeight(int &width, int &height) const
	{
		width  = mWidth;
		height = mHeight;
	}

	/**
	*  @brief
	*    Returns the distance (in pixel) from the current pen position to the glyph bitmap
	*/
	inline void IFontGlyph::getCorner(float &cornerX, float &cornerY) const
	{
		cornerX = mCornerX;
		cornerY = mCornerY;
	}

	/**
	*  @brief
	*    Return the object space pen advance
	*/
	inline void IFontGlyph::getPenAdvance(float &penAdvanceX, float &penAdvanceY) const
	{
		penAdvanceX = mPenAdvanceX;
		penAdvanceY = mPenAdvanceY;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Default constructor
	*/
	inline IFontGlyph::IFontGlyph() :
		mWidth(0),
		mHeight(0),
		mCornerX(0.0f),
		mCornerY(0.0f),
		mPenAdvanceX(0.0f),
		mPenAdvanceY(0.0f)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	inline IFontGlyph::IFontGlyph(const IFontGlyph &)
	{
		// Not supported
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline IFontGlyph &IFontGlyph::operator =(const IFontGlyph &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
