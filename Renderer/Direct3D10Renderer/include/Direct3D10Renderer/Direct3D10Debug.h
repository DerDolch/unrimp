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
#ifndef __DIRECT3D10RENDERER_DIRECT3D10DEBUG_H__
#define __DIRECT3D10RENDERER_DIRECT3D10DEBUG_H__


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
/*
*  @brief
*    Check whether or not the given resource is owned by the given renderer
*
*  @note
*    In case of no match, a debug output message will be made with a following immediate "return"
*/
#ifdef DIRECT3D10RENDERER_NO_RENDERERMATCHCHECK
	#define DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(rendererReference, resourceReference)
#else
	#define DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(rendererReference, resourceReference) \
		if (&rendererReference != &(resourceReference).getRenderer()) \
		{ \
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: The given resource is owned by another renderer instance") \
			return; \
		}
#endif


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_DIRECT3D10DEBUG_H__
