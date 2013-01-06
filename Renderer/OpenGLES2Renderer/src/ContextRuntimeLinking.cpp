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
#define CONTEXT_DEFINERUNTIMELINKING

#include "OpenGLES2Renderer/ContextRuntimeLinking.h"
#include "OpenGLES2Renderer/ExtensionsRuntimeLinking.h"
#ifdef LINUX
	#include <dlfcn.h> // for dlopen, dlclose and co
	#include <link.h> // for getting the path to the library (for the error message)
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ContextRuntimeLinking::ContextRuntimeLinking(handle nativeWindowHandle) :
		IContext(nativeWindowHandle),
		mEGLSharedLibrary(nullptr),
		mGLESSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mExtensions(new ExtensionsRuntimeLinking())
	{
		// Load the shared libraries
		if (loadSharedLibraries())
		{
			// Load the EGL entry points
			if (loadEGLEntryPoints())
			{
				// Load the OpenGL ES 2 entry points
				if (loadGLESEntryPoints())
				{
					// Entry points successfully registered
					mEntryPointsRegistered = true;
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Failed to load in the OpenGL ES 2 entry points\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Failed to load in the EGL entry points\n")
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Failed to load in the OpenGL ES 2 shared libraries\n")
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ContextRuntimeLinking::~ContextRuntimeLinking()
	{
		// De-initialize the context while we still can
		deinitialize();

		// Destroy the extensions instance
		delete mExtensions;

		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mEGLSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mEGLSharedLibrary));
			}
			if (nullptr != mGLESSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mGLESSharedLibrary));
			}
		#elif defined LINUX
			if (nullptr != mEGLSharedLibrary)
			{
				::dlclose(mEGLSharedLibrary);
			}
			if (nullptr != mGLESSharedLibrary)
			{
				::dlclose(mGLESSharedLibrary);
			}
		#else
			#error "Unsupported platform"
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES2Renderer::IContext methods    ]
	//[-------------------------------------------------------]
	bool ContextRuntimeLinking::initialize(unsigned int multisampleAntialiasingSamples)
	{
		// Entry points successfully registered?
		if (mEntryPointsRegistered)
		{
			// Call base implementation
			if (IContext::initialize(multisampleAntialiasingSamples))
			{
				// Initialize the supported extensions
				mExtensions->initialize();

				// Done
				return true;
			}
		}

		// Error!
		return false;
	}

	const IExtensions &ContextRuntimeLinking::getExtensions() const
	{
		return *mExtensions;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual OpenGLES2Renderer::IContext methods ]
	//[-------------------------------------------------------]
	EGLConfig ContextRuntimeLinking::chooseConfig(unsigned int multisampleAntialiasingSamples) const
	{
		// Try to find a working EGL configuration
		EGLConfig eglConfig = nullptr;
		EGLint numberOfConfigurations = 0;
		bool chooseConfigCapitulated = false;
		bool multisampleAntialiasing = false;
		EGLint multisampleAntialiasingSamplesCurrent = static_cast<EGLint>(multisampleAntialiasingSamples);
		do
		{
			// Get the current multisample antialiasing settings
			multisampleAntialiasing = (multisampleAntialiasingSamplesCurrent > 1);	// Multisample antialiasing with just one sample per per pixel isn't real multisample, is it? :D
			// const EGLint multisampleAntialiasingSampleBuffers = multisampleAntialiasing ? 1 : 0;

			// Set desired configuration
			const EGLint configAttribs[] =
			{
				EGL_LEVEL,				0,										// Frame buffer level
				EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,							// Which types of EGL surfaces are supported
				EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,						// Which client APIs are supported
				EGL_RED_SIZE,			8,										// Bits of red color component
				EGL_GREEN_SIZE,			8,										// Bits of red color component
				EGL_BLUE_SIZE,			8,										// Bits of red color component
				EGL_DEPTH_SIZE,			16,										// Bits of Z in the depth buffer TODO(co) Make it possible to set this from the outside, but do also automatically go down if it fails, e.g. 24 doesn't work for me
				// TODO(co) Currently something looks wrong when using the desktop drivers - just black screen when using multisample ("AMD Catalyst 11.8" on a "ATI Mobility Radeon HD 4850")
				//       -> No issues with Android on the device (but it looks like there's no antialiasing, check it later in detail)
			//	EGL_SAMPLE_BUFFERS,		multisampleAntialiasingSampleBuffers,	// Number of multisample buffers (enable/disable multisample antialiasing)
			//	EGL_SAMPLES,			multisampleAntialiasingSamplesCurrent,	// Number of samples per pixel (multisample antialiasing samples)
				EGL_NONE
			};

			// Choose exactly one matching configuration
			if (eglChooseConfig(mDisplay, configAttribs, &eglConfig, 1, &numberOfConfigurations) == EGL_FALSE || numberOfConfigurations < 1)
			{
				// Can we change something on the multisample antialiasing? (may be the cause that no configuration was found!)
				if (multisampleAntialiasing)
				{
					if (multisampleAntialiasingSamplesCurrent > 8)
					{
						multisampleAntialiasingSamplesCurrent = 8;
					}
					else if (multisampleAntialiasingSamplesCurrent > 4)
					{
						multisampleAntialiasingSamplesCurrent = 4;
					}
					else if (multisampleAntialiasingSamplesCurrent > 2)
					{
						multisampleAntialiasingSamplesCurrent = 2;
					}
					else if (2 == multisampleAntialiasingSamplesCurrent)
					{
						multisampleAntialiasingSamplesCurrent = 0;
					}
				}
				else
				{
					// Don't mind, forget it...
					chooseConfigCapitulated = true;
				}
			}
		} while (numberOfConfigurations < 1 && !chooseConfigCapitulated);

		// Done
		return eglConfig;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Loads the EGL shared library
	*/
	bool ContextRuntimeLinking::loadSharedLibraries()
	{
		// We don't need to check m_pEGLSharedLibrary and m_pGLESSharedLibrary at this point because we know they must contain a null pointer

		// EGL and OpenGL ES 2 may be within a single shared library, or within two separate shared libraries
		#ifdef WIN32
			// First, try the OpenGL ES 2 emulator from ARM (it's possible to move around this dll without issues, so, this one first)
			mEGLSharedLibrary = ::LoadLibraryExA("libEGL.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mEGLSharedLibrary)
			{
				mGLESSharedLibrary = ::LoadLibraryExA("libGLESv2.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			}
			else
			{
				// Second, try the AMD/ATI driver
				mEGLSharedLibrary = ::LoadLibraryExA("atioglxx.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mEGLSharedLibrary)
				{
					mGLESSharedLibrary = ::LoadLibraryExA("atioglxx.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				}
				else
				{
					// Third, try the NVIDIA driver
					mEGLSharedLibrary = ::LoadLibraryExA("nvoglv32.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr != mEGLSharedLibrary)
					{
						mGLESSharedLibrary = ::LoadLibraryExA("nvoglv32.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					}
				}
			}
		#elif defined LINUX
			// First "libGL.so": The closed source drivers doesn't provide separate libraries for GLES and EGL (at least the drivers from AMD)
			// but the separate EGL/GLES2 libs might be present on the system
			mEGLSharedLibrary = ::dlopen("libGL.so", RTLD_LAZY);
			if (nullptr != mEGLSharedLibrary)
			{
				// Try finding the eglGetProcAddress to determine if this library contains EGL/GLES support.
				// This check is needed because only the closed source drivers have the EGL/GLES support in "libGL.so".
				// The open source drivers (mesa) have separate libraries for this and they can be present on the system even the closed source drivers are used.
				void *pSymbol = dlsym(mEGLSharedLibrary, "eglGetProcAddress");
				if (pSymbol)
					mGLESSharedLibrary = ::dlopen("libGL.so", RTLD_LAZY);
				else {
					// Unload the library
					::dlclose(mEGLSharedLibrary);
					mEGLSharedLibrary = nullptr;
				}
			}
			if (nullptr == mEGLSharedLibrary)
			{
				// Then we try the separate libs for EGL and GLES (provided either by an emulator or native from mesa)
				mEGLSharedLibrary = ::dlopen("libEGL.so", RTLD_LAZY);
				if (nullptr != mEGLSharedLibrary)
				{
					mGLESSharedLibrary = ::dlopen("libGLESv2.so", RTLD_LAZY);
				}
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mEGLSharedLibrary && nullptr != mGLESSharedLibrary);
	}

	/**
	*  @brief
	*    Loads the EGL entry points
	*/
	bool ContextRuntimeLinking::loadEGLEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mEGLSharedLibrary), #funcName);																			\
					if (nullptr == symbol)																																			\
					{																																								\
						/* The specification states that "eglGetProcAddress" is only for extension functions, but when using OpenGL ES 2 on desktop PC by using a					\
						   native OpenGL ES 2 capable graphics driver under Linux (tested with "AMD Catalyst 11.8"), only this way will work */										\
						if (nullptr != eglGetProcAddress)																															\
						{																																							\
							symbol = eglGetProcAddress(#funcName);																													\
						}																																							\
					}																																								\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						wchar_t moduleFilename[MAX_PATH];																															\
						moduleFilename[0] = '\0';																																	\
						::GetModuleFileNameW(static_cast<HMODULE>(mEGLSharedLibrary), moduleFilename, MAX_PATH);																	\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Failed to locate the entry point \"%s\" within the EGL shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																				\
					}																																								\
				}
		#elif defined LINUX
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::dlsym(mEGLSharedLibrary, #funcName);																			\
					if (nullptr == symbol)																																			\
					{																																								\
						/* The specification states that "eglGetProcAddress" is only for extension functions, but when using OpenGL ES 2 on desktop PC by using a					\
						   native OpenGL ES 2 capable graphics driver under Linux (tested with "AMD Catalyst 11.8"), only this way will work */										\
						if (nullptr != eglGetProcAddress)																															\
						{																																							\
							symbol = eglGetProcAddress(#funcName);																													\
						}																																							\
					}																																								\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						link_map *LinkMap = nullptr; \
						const char* libName = "unknown"; \
						if (dlinfo(mEGLSharedLibrary, RTLD_DI_LINKMAP, &LinkMap)) \
						{ \
							libName = LinkMap->l_name; \
						} \
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Failed to locate the entry point \"%s\" within the EGL shared library \"%s\"", #funcName, libName)	\
						result = false;																																				\
					}																																								\
				}
		#else
			#error "Unsupported platform"
		#endif


		// Load the entry points
		IMPORT_FUNC(eglGetProcAddress);
		IMPORT_FUNC(eglGetError);
		IMPORT_FUNC(eglGetDisplay);
		IMPORT_FUNC(eglInitialize);
		IMPORT_FUNC(eglTerminate);
		IMPORT_FUNC(eglQueryString);
		IMPORT_FUNC(eglGetConfigs);
		IMPORT_FUNC(eglChooseConfig);
		IMPORT_FUNC(eglGetConfigAttrib);
		IMPORT_FUNC(eglCreateWindowSurface);
		IMPORT_FUNC(eglDestroySurface);
		IMPORT_FUNC(eglQuerySurface);
		IMPORT_FUNC(eglBindAPI);
		IMPORT_FUNC(eglQueryAPI);
		IMPORT_FUNC(eglWaitClient);
		IMPORT_FUNC(eglReleaseThread);
		IMPORT_FUNC(eglSurfaceAttrib);
		IMPORT_FUNC(eglBindTexImage);
		IMPORT_FUNC(eglReleaseTexImage);
		IMPORT_FUNC(eglSwapInterval);
		IMPORT_FUNC(eglCreateContext);
		IMPORT_FUNC(eglDestroyContext);
		IMPORT_FUNC(eglMakeCurrent);
		IMPORT_FUNC(eglGetCurrentContext);
		IMPORT_FUNC(eglGetCurrentSurface);
		IMPORT_FUNC(eglGetCurrentDisplay);
		IMPORT_FUNC(eglQueryContext);
		IMPORT_FUNC(eglWaitGL);
		IMPORT_FUNC(eglWaitNative);
		IMPORT_FUNC(eglSwapBuffers);
		IMPORT_FUNC(eglCopyBuffers);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	/**
	*  @brief
	*    Loads the OpenGL ES 2 entry points
	*/
	bool ContextRuntimeLinking::loadGLESEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef ANDROID
			// Native OpenGL ES 2 on mobile device
			#error "Unsupported platform"	// Load from mGLESSharedLibrary
		#else
			// Native OpenGL ES 2 on desktop PC, we need an function entry point work around for this
			#define IMPORT_FUNC(funcName)																														\
				if (result)																																		\
				{																																				\
					/* The specification states that "eglGetProcAddress" is only for extension functions, but when using OpenGL ES 2 on desktop PC by using a	\
					   native OpenGL ES 2 capable graphics driver (tested with "AMD Catalyst 11.8"), only this way will work */									\
					void *symbol = eglGetProcAddress(#funcName);																								\
					if (nullptr != symbol)																														\
					{																																			\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																						\
					}																																			\
					else																																		\
					{																																			\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Failed to locate the entry point \"%s\" within the GLES shared library", #funcName)	\
						result = false;																															\
					}																																			\
				}
		#endif

		// Load the entry points
		IMPORT_FUNC(glActiveTexture);
		IMPORT_FUNC(glAttachShader);
		IMPORT_FUNC(glBindAttribLocation);
		IMPORT_FUNC(glBindBuffer);
		IMPORT_FUNC(glBindFramebuffer);
		IMPORT_FUNC(glBindRenderbuffer);
		IMPORT_FUNC(glBindTexture);
		IMPORT_FUNC(glBlendColor);
		IMPORT_FUNC(glBlendEquation);
		IMPORT_FUNC(glBlendEquationSeparate);
		IMPORT_FUNC(glBlendFunc);
		IMPORT_FUNC(glBlendFuncSeparate);
		IMPORT_FUNC(glBufferData);
		IMPORT_FUNC(glBufferSubData);
		IMPORT_FUNC(glCheckFramebufferStatus);
		IMPORT_FUNC(glClear);
		IMPORT_FUNC(glClearColor);
		IMPORT_FUNC(glClearDepthf);
		IMPORT_FUNC(glClearStencil);
		IMPORT_FUNC(glColorMask);
		IMPORT_FUNC(glCompileShader);
		IMPORT_FUNC(glCompressedTexImage2D);
		IMPORT_FUNC(glCompressedTexSubImage2D);
		IMPORT_FUNC(glCopyTexImage2D);
		IMPORT_FUNC(glCopyTexSubImage2D);
		IMPORT_FUNC(glCreateProgram);
		IMPORT_FUNC(glCreateShader);
		IMPORT_FUNC(glCullFace);
		IMPORT_FUNC(glDeleteBuffers);
		IMPORT_FUNC(glDeleteFramebuffers);
		IMPORT_FUNC(glDeleteProgram);
		IMPORT_FUNC(glDeleteRenderbuffers);
		IMPORT_FUNC(glDeleteShader);
		IMPORT_FUNC(glDeleteTextures);
		IMPORT_FUNC(glDepthFunc);
		IMPORT_FUNC(glDepthMask);
		IMPORT_FUNC(glDepthRangef);
		IMPORT_FUNC(glDetachShader);
		IMPORT_FUNC(glDisable);
		IMPORT_FUNC(glDisableVertexAttribArray);
		IMPORT_FUNC(glDrawArrays);
		IMPORT_FUNC(glDrawElements);
		IMPORT_FUNC(glEnable);
		IMPORT_FUNC(glEnableVertexAttribArray);
		IMPORT_FUNC(glFinish);
		IMPORT_FUNC(glFlush);
		IMPORT_FUNC(glFramebufferRenderbuffer);
		IMPORT_FUNC(glFramebufferTexture2D);
		IMPORT_FUNC(glFrontFace);
		IMPORT_FUNC(glGenBuffers);
		IMPORT_FUNC(glGenerateMipmap);
		IMPORT_FUNC(glGenFramebuffers);
		IMPORT_FUNC(glGenRenderbuffers);
		IMPORT_FUNC(glGenTextures);
		IMPORT_FUNC(glGetActiveAttrib);
		IMPORT_FUNC(glGetActiveUniform);
		IMPORT_FUNC(glGetAttachedShaders);
		IMPORT_FUNC(glGetAttribLocation);
		IMPORT_FUNC(glGetBooleanv);
		IMPORT_FUNC(glGetBufferParameteriv);
		IMPORT_FUNC(glGetError);
		IMPORT_FUNC(glGetFloatv);
		IMPORT_FUNC(glGetFramebufferAttachmentParameteriv);
		IMPORT_FUNC(glGetIntegerv);
		IMPORT_FUNC(glGetProgramiv);
		IMPORT_FUNC(glGetProgramInfoLog);
		IMPORT_FUNC(glGetRenderbufferParameteriv);
		IMPORT_FUNC(glGetShaderiv);
		IMPORT_FUNC(glGetShaderInfoLog);
		IMPORT_FUNC(glGetShaderPrecisionFormat);
		IMPORT_FUNC(glGetShaderSource);
		IMPORT_FUNC(glGetString);
		IMPORT_FUNC(glGetTexParameterfv);
		IMPORT_FUNC(glGetTexParameteriv);
		IMPORT_FUNC(glGetUniformfv);
		IMPORT_FUNC(glGetUniformiv);
		IMPORT_FUNC(glGetUniformLocation);
		IMPORT_FUNC(glGetVertexAttribfv);
		IMPORT_FUNC(glGetVertexAttribiv);
		IMPORT_FUNC(glGetVertexAttribPointerv);
		IMPORT_FUNC(glHint);
		IMPORT_FUNC(glIsBuffer);
		IMPORT_FUNC(glIsEnabled);
		IMPORT_FUNC(glIsFramebuffer);
		IMPORT_FUNC(glIsProgram);
		IMPORT_FUNC(glIsRenderbuffer);
		IMPORT_FUNC(glIsShader);
		IMPORT_FUNC(glIsTexture);
		IMPORT_FUNC(glLineWidth);
		IMPORT_FUNC(glLinkProgram);
		IMPORT_FUNC(glPixelStorei);
		IMPORT_FUNC(glPolygonOffset);
		IMPORT_FUNC(glReadPixels);
		IMPORT_FUNC(glReleaseShaderCompiler);
		IMPORT_FUNC(glRenderbufferStorage);
		IMPORT_FUNC(glSampleCoverage);
		IMPORT_FUNC(glScissor);
		IMPORT_FUNC(glShaderBinary);
		IMPORT_FUNC(glShaderSource);
		IMPORT_FUNC(glStencilFunc);
		IMPORT_FUNC(glStencilFuncSeparate);
		IMPORT_FUNC(glStencilMask);
		IMPORT_FUNC(glStencilMaskSeparate);
		IMPORT_FUNC(glStencilOp);
		IMPORT_FUNC(glStencilOpSeparate);
		IMPORT_FUNC(glTexImage2D);
		IMPORT_FUNC(glTexParameterf);
		IMPORT_FUNC(glTexParameterfv);
		IMPORT_FUNC(glTexParameteri);
		IMPORT_FUNC(glTexParameteriv);
		IMPORT_FUNC(glTexSubImage2D);
		IMPORT_FUNC(glUniform1f);
		IMPORT_FUNC(glUniform1fv);
		IMPORT_FUNC(glUniform1i);
		IMPORT_FUNC(glUniform1iv);
		IMPORT_FUNC(glUniform2f);
		IMPORT_FUNC(glUniform2fv);
		IMPORT_FUNC(glUniform2i);
		IMPORT_FUNC(glUniform2iv);
		IMPORT_FUNC(glUniform3f);
		IMPORT_FUNC(glUniform3fv);
		IMPORT_FUNC(glUniform3i);
		IMPORT_FUNC(glUniform3iv);
		IMPORT_FUNC(glUniform4f);
		IMPORT_FUNC(glUniform4fv);
		IMPORT_FUNC(glUniform4i);
		IMPORT_FUNC(glUniform4iv);
		IMPORT_FUNC(glUniformMatrix2fv);
		IMPORT_FUNC(glUniformMatrix3fv);
		IMPORT_FUNC(glUniformMatrix4fv);
		IMPORT_FUNC(glUseProgram);
		IMPORT_FUNC(glValidateProgram);
		IMPORT_FUNC(glVertexAttrib1f);
		IMPORT_FUNC(glVertexAttrib1fv);
		IMPORT_FUNC(glVertexAttrib2f);
		IMPORT_FUNC(glVertexAttrib2fv);
		IMPORT_FUNC(glVertexAttrib3f);
		IMPORT_FUNC(glVertexAttrib3fv);
		IMPORT_FUNC(glVertexAttrib4f);
		IMPORT_FUNC(glVertexAttrib4fv);
		IMPORT_FUNC(glVertexAttribPointer);
		IMPORT_FUNC(glViewport);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
