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
#include "OpenGLRenderer/Linux/ContextLinux.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include <iostream>



//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ContextLinux::ContextLinux(handle nativeWindowHandle) :
		mOpenGLRuntimeLinking(new OpenGLRuntimeLinking()),
		mNativeWindowHandle(nativeWindowHandle),
		mDummyWindow(NULL_HANDLE),
		mDisplay(nullptr),
		m_pDummyVisualInfo(nullptr),
		mWindowRenderContext(NULL_HANDLE)
	{
		// Is OpenGL available?
		if (mOpenGLRuntimeLinking->isOpenGLAvaiable())
		{
			// Get X server display connection
			mDisplay = XOpenDisplay(nullptr);
			if (mDisplay) {
				// Get an appropriate visual
				int nAttributeList[] = {
					GLX_RGBA,
					GLX_DOUBLEBUFFER,
					GLX_RED_SIZE,	4,
					GLX_GREEN_SIZE,	4,
					GLX_BLUE_SIZE,	4,
					GLX_DEPTH_SIZE,	16,
					0 };	// = "None"

				m_pDummyVisualInfo = glXChooseVisual(mDisplay, DefaultScreen(mDisplay), nAttributeList);
				if (m_pDummyVisualInfo) {
					
					if (NULL_HANDLE == mNativeWindowHandle) {
						// Create a color map
						XSetWindowAttributes sSetWindowAttributes;
						sSetWindowAttributes.colormap = XCreateColormap(mDisplay, RootWindow(mDisplay, m_pDummyVisualInfo->screen), m_pDummyVisualInfo->visual, AllocNone);

						// Create a window
						sSetWindowAttributes.border_pixel = 0;
						sSetWindowAttributes.event_mask = 0;
						mNativeWindowHandle = mDummyWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, m_pDummyVisualInfo->screen), 0, 0, 300,
															300, 0, m_pDummyVisualInfo->depth, InputOutput, m_pDummyVisualInfo->visual,
															CWBorderPixel|CWColormap|CWEventMask, &sSetWindowAttributes);
						std::cout<<"Create dummy window\n";
					}
					
					// Create a GLX context
					GLXContext legacyRenderContext = glXCreateContext(mDisplay, m_pDummyVisualInfo, 0, GL_TRUE);
					if (legacyRenderContext) {
						// Make the internal dummy to the current render target
						int result = glXMakeCurrent(mDisplay, mNativeWindowHandle, legacyRenderContext);
						std::cout<<"make legacy context current: "<<result<<"\n";
						
						// Create the render context of the OpenGL window
						mWindowRenderContext = createOpenGLContext();

						// Destroy the legacy OpenGL render context
						glXMakeCurrent(mDisplay, None, nullptr);
						glXDestroyContext(mDisplay, legacyRenderContext);

						// If there's an OpenGL context, do some final initialization steps
						if (NULL_HANDLE != mWindowRenderContext)
						{
							
							// Make the OpenGL context to the current one
							int result = glXMakeCurrent(mDisplay, mNativeWindowHandle, mWindowRenderContext);
							std::cout<<"make new context current: "<<result<<"\n";
							std::cout<<"supported extensions: "<<glGetString(GL_EXTENSIONS)<<"\n";
						}
						
					} else {
						// Error, failed to create a GLX context!
					}
				} else {
					// Error, failed to get an appropriate visual!
				}
			} else {
				// Error, failed to get display!
			}

			// Is there a valid render context?
			if (nullptr != mWindowRenderContext)
			{
				// Initialize the OpenGL extensions
				getExtensions().initialize();

				#ifdef RENDERER_OUTPUT_DEBUG
					// "GL_ARB_debug_output"-extension available?
					if (getExtensions().isGL_ARB_debug_output())
					{
						// Synchronous debug output, please
						// -> Makes it easier to find the place causing the issue
						glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

						// We don't need to configure the debug output by using "glDebugMessageControlARB()",
						// by default all messages are enabled and this is good this way

						// Set the debug message callback function
						glDebugMessageCallbackARB(&ContextLinux::debugMessageCallback, nullptr);
					}
				#endif
			}
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ContextLinux::~ContextLinux()
	{
		// Release the device context of the OpenGL window
		if (mDisplay)
		{
			// Is the device context of the OpenGL window is the currently active OpenGL device context?
			if (glXGetCurrentContext() == mWindowRenderContext)
			{
				glXMakeCurrent(mDisplay, None, nullptr);
			}

			// Destroy the render context of the OpenGL window
			if (NULL_HANDLE != mWindowRenderContext)
			{
				glXDestroyContext(mDisplay, mWindowRenderContext);
			}
		}

		// Destroy the OpenGL dummy window, in case there's one
		if (NULL_HANDLE != mDummyWindow)
		{
			// Destroy the OpenGL dummy window
			::XDestroyWindow(mDisplay, mDummyWindow);
		}

		// Destroy the OpenGL runtime linking instance
		delete mOpenGLRuntimeLinking;
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Debug message callback function called by the "GL_ARB_debug_output"-extension
	*/
	void ContextLinux::debugMessageCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int, const char *message, void *)
	{
		// Source to string
		char debugSource[16];
		switch (source)
		{
			case GL_DEBUG_SOURCE_API_ARB:
				strncpy(debugSource, "OpenGL", 7);
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				strncpy(debugSource, "Linux", 8);
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				strncpy(debugSource, "Shader compiler", 16);
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
				strncpy(debugSource, "Third party", 12);
				break;

			case GL_DEBUG_SOURCE_APPLICATION_ARB:
				strncpy(debugSource, "Application", 12);
				break;

			case GL_DEBUG_SOURCE_OTHER_ARB:
				strncpy(debugSource, "Other", 6);
				break;

			default:
				strncpy(debugSource, "?", 1);
				break;
		}

		// Debug type to string
		char debugType[20];
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR_ARB:
				strncpy(debugType, "Error", 6);
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
				strncpy(debugType, "Deprecated behavior", 20);
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
				strncpy(debugType, "Undefined behavior", 19);
				break;

			case GL_DEBUG_TYPE_PORTABILITY_ARB:
				strncpy(debugType, "Portability", 12);
				break;

			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
				strncpy(debugType, "Performance", 12);
				break;

			case GL_DEBUG_TYPE_OTHER_ARB:
				strncpy(debugType, "Other", 6);
				break;

			default:
				strncpy(debugType, "?", 1);
				break;
		}

		// Debug severity to string
		char debugSeverity[7];
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH_ARB:
				strncpy(debugSeverity, "High", 5);
				break;

			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
				strncpy(debugSeverity, "Medium", 7);
				break;

			case GL_DEBUG_SEVERITY_LOW_ARB:
				strncpy(debugSeverity, "Low", 3);
				break;

			default:
				strncpy(debugType, "?", 1);
				break;
		}

		// Output the debug message
		#ifdef _DEBUG
			RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: OpenGL debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"\n", debugSource, debugType, id, debugSeverity, message)
		#else
			// Avoid "warning C4100: '<x>' : unreferenced formal parameter"-warning
			id = id;
			message = message;
		#endif
	}


	static bool ctxErrorOccurred = false;
	static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
	{
		ctxErrorOccurred = true;
		return 0;
	}
	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Create a OpenGL context
	*/
	GLXContext ContextLinux::createOpenGLContext()
	{
		#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
		#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092
		// Get the available GLX extensions as string
		const char *extensions = glXQueryExtensionsString(mDisplay, XDefaultScreen(mDisplay));

		// Check whether or not "GLX_ARB_create_context" is a substring of the GLX extension string meaning that this OpenGL extension is supported
		if (nullptr != strstr(extensions, "GLX_ARB_create_context"))
		{
			typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
			// Get the OpenGL extension "glXCreateContextAttribsARB" function pointer
			GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<GLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB"));
			if (nullptr != glXCreateContextAttribsARB)
			{
				ctxErrorOccurred = false;
				int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);
				
				// OpenGL 3.1 - required for "gl_InstanceID" within shaders
				// Create the OpenGL context
				int ATTRIBUTES[] =
				{
					// We want an OpenGL context
					GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
					GLX_CONTEXT_MINOR_VERSION_ARB, 1,
					// -> "GLX_CONTEXT_DEBUG_BIT_ARB" comes from the "GL_ARB_debug_output"-extension
					// TODO(co) Make it possible to activate "GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB" from the outside
					#ifdef RENDERER_OUTPUT_DEBUG
					//	GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
					//	GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
					#else
					//	GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
					#endif
					// Done
					0
				};
				
				int nelements;
				int visual_attribs[] =
				{
					GLX_RENDER_TYPE, GLX_RGBA_BIT,
					GLX_DOUBLEBUFFER, true,
					GLX_RED_SIZE, 1,
					GLX_GREEN_SIZE, 1,
					GLX_BLUE_SIZE, 1,
					None
				};
				GLXFBConfig *fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), visual_attribs, &nelements);
				std::cout<<"Renderer: Got "<<nelements<<" of FBCOnfig\n";
				GLXContext glxContext = glXCreateContextAttribsARB(mDisplay, *fbc, 0, true, ATTRIBUTES);
				
				XSync( mDisplay, False );
				
				// TODO(sw) make this fallback optional (via an option)
				if (ctxErrorOccurred)
				{
					std::cerr<<"could not create opengl 3+ context try creating pre 3+ context\n";
					ctxErrorOccurred = false;
					
					// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
					ATTRIBUTES[1] = 1;
					// GLX_CONTEXT_MINOR_VERSION_ARB = 0
					ATTRIBUTES[3] = 0;
					glxContext = glXCreateContextAttribsARB(mDisplay, *fbc, 0, true, ATTRIBUTES);
					
					// Sync to ensure any errors generated are processed.
					XSync( mDisplay, False );
					
					// Restore the original error handler
					XSetErrorHandler( oldHandler );
				}
				
				
				if (nullptr != glxContext)
				{
					std::cout<<"Renderer: OGL Context with glXCreateContextAttribsARB created\n";
					// Done
					return glxContext;
				}
				else
				{
					std::cerr<<"Renderer: Could not create OGL Context with glXCreateContextAttribsARB\n";
					// Error, context creation failed!
					return NULL_HANDLE;
				}
			}
			else
			{
				std::cerr<<"Renderer: could not found glXCreateContextAttribsARB\n";
				// Error, failed to obtain the "GLX_ARB_create_context" function pointer (wow, something went terrible wrong!)
				return NULL_HANDLE;
			}
		}
		else
		{
			std::cerr<<"Renderer: GLX_ARB_create_context not supported\n";
			// Error, the OpenGL extension "GLX_ARB_create_context" is not supported... as a result we can't create an OpenGL context!
			return NULL_HANDLE;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
