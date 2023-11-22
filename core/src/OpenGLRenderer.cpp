#include "../include/OpenGLRenderer.h"

// Define and include stb image loader 
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;

PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUNIFORM2FPROC glUniform2f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLUNIFORM1IVPROC glUniform1iv = nullptr;
PFNGLUNIFORM2IVPROC glUniform2iv = nullptr;
PFNGLUNIFORM3IVPROC glUniform3iv = nullptr;
PFNGLUNIFORM4IVPROC glUniform4iv = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = nullptr;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = nullptr;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = nullptr;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = nullptr;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = nullptr;

// Shader
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;

// VBO
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC	glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC	glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = nullptr;


PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;

#if defined (_WINDOWS)
#include "../include/GL/wglext.h"
PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2D glCompressedTexImage2D = nullptr;
PFNGLBLENDCOLORPROC glBlendColor = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;

LRESULT CALLBACK InitWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			24, 8, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		HDC dc = GetDC(hWnd);

		int pixelFormat = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, pixelFormat, &pfd);

		HGLRC rc = wglCreateContext(dc);
		wglMakeCurrent(dc, rc);

		// Now that we have a current context, we can initialize opengl functions
		OpenGLRenderer::InitFunctions();

		// Functions are now initialized, all done here
		wglDeleteContext(rc);
		ReleaseDC(hWnd, dc);
		PostQuitMessage(0);
	}
	break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
#endif

#if defined (_LINUX)
#include <X11/Xlib.h>
#include <GL/glx.h>
//#include <GL/glu.h>
#endif

OpenGLRenderer::OpenGLRenderer() :
	m_Context(nullptr)
{
#if defined (_WINDOWS)
	m_hRC = nullptr;
#endif
}

OpenGLRenderer::~OpenGLRenderer()
{
#if defined (_WINDOWS)
	if (m_Context)
	{
		wglMakeCurrent(m_Context, nullptr); // Set active context
		// Release context resources when shutting down application
		wglDeleteContext(m_hRC);
		::ReleaseDC(IApplication::GetApp()->GetWindow(), m_Context);
		m_Context = nullptr;
	}
#endif
}

bool OpenGLRenderer::Create()
{
	const int32_t multisampleCount = 8;

#if defined (_WINDOWS)
	// Create dummy window for initializing the wgl functions for pixel format
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = InitWndProc;
	wc.hInstance = ::GetModuleHandle(NULL);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"wgl_init_dummy_window";
	wc.style = CS_OWNDC;
	if (!::RegisterClass(&wc))
	{
		return false;
	}
	HWND w = CreateWindowW(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, -32, -32, 32, 32, 0, 0, wc.hInstance, 0);
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		DispatchMessage(&msg);
	}
	::DestroyWindow(w);


	// Dummy window has finished and now we have access to all wgl functions
	// to retrieve the advanced pixel formats
	m_Context = ::GetDC(IApplication::GetApp()->GetWindow());

	uint32_t flags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		flags,				// Flags
		PFD_TYPE_RGBA,		// The type of framebuffer. RGBA or palette
		32,					// Number of bits for the framebuffer
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,					// Number of bits for the depthbuffer
		8,					// Number of bits for the stencilbuffer
		0,					// Number of Aux buffers in the framebuffer
		PFD_MAIN_PLANE,		// Set this as main rendering context
		0,
		0, 0, 0
	};

	if (wglChoosePixelFormatARB)
	{
		int attribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, multisampleCount, // Number of samples
			0
		};

		int pixelFormat = -1;
		UINT numFormats;

		// Choose pixel format with multisampling
		wglChoosePixelFormatARB(m_Context, attribList, NULL, 1, &pixelFormat, &numFormats);
		if (numFormats > 0 && pixelFormat != -1)
		{
			// Check if setting pixel format to device context is succesful
			if (!::SetPixelFormat(m_Context, pixelFormat, &pfd))
			{
				return false;
			}
		}
		else
		{
			// Fall back to old pixel format
			int pixelFormat = ::ChoosePixelFormat(m_Context, &pfd);
			if (pixelFormat == 0)
			{
				return false;
			}

			if (!::SetPixelFormat(m_Context, pixelFormat, &pfd))
			{
				return false;
			}
		}
	}

	m_hRC = wglCreateContext(m_Context);
	// Activate resource context so that all rendering will be done to it
	wglMakeCurrent(m_Context, m_hRC);

#endif	// #if defined (_WINDOWS)

#if defined (_LINUX)
	int depthBufferBits = 24;
	int stencilBufferBits = 8;
	int doubleBufferedAttribList[] =
	{
		GLX_RGBA, GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 4,
		GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, depthBufferBits,
		GLX_STENCIL_SIZE, stencilBufferBits,
		None
	};

	XVisualInfo* vi = nullptr;

	// Attempt to create a double buffered window
	Display* display = IApplication::GetApp()->GetDisplay();
	int screen = DefaultScreen(display);

	int n = 0;
	// Get a framebuffer config using the default attributes
	GLXFBConfig framebufferConfig = (*glXChooseFBConfig(display, screen, 0, &n));
	if (!framebufferConfig)
	{
		IApplication::Debug("OpenGLRenderer: no framebufferconfig");
	}

	vi = glXChooseVisual(display, screen, doubleBufferedAttribList);
	if (!vi)
	{
		IApplication::Debug("OpenGLRenderer: glXChooseVisual failed");
	}

	//IApplication::Debug("OpenGLRenderer: glXChooseVisual complete");

	// Create a GL 2.1 context
	m_Context = glXCreateContext(display, vi, 0, GL_TRUE);
	if (!m_Context)
	{
		IApplication::Debug("OpenGLRenderer: glXCreateContext failed");
		return false;
	}

	//IApplication::Debug("OpenGLRenderer: glXCreateContext complete");

	glXMakeCurrent(display, (GLXDrawable)IApplication::GetApp()->GetWindow(), (GLXContext)m_Context);
	InitFunctions();
#endif

	// Set initial stage to OpenGL
	SetDefaultSettings();
	// Enable multisampling
	if (multisampleCount)
	{
		glEnable(GL_MULTISAMPLE);
	}

	return true;
}

void OpenGLRenderer::Flip()
{
	glFlush(); // Finalize all commands in the driver

#if defined (_WINDOWS)
	SwapBuffers(m_Context); // Transfer graphics to the screen
#endif

#if defined (_LINUX)
	Display* display = IApplication::GetApp()->GetDisplay();
	Window wnd = IApplication::GetApp()->GetWindow();
	glXSwapBuffers(display, wnd);
#endif

}

void OpenGLRenderer::Clear(float r, float g, float b, float a, float depth, int32_t stencil)
{
	glClearDepthf(depth);
	glClearStencil(stencil);

	// Clear background colorbuffer
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::SetViewport(const glm::ivec4& area)
{
	glViewport(area.x, area.y, area.z, area.w);
}

bool OpenGLRenderer::SetTexture(uint32_t program, uint32_t texture, int32_t slot, const std::string_view& uniformName)
{
	// Activate correct texture slot in the GPU
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
	const GLint location = glGetUniformLocation(program, uniformName.data());
	if (location >= 0)
	{
		glUniform1i(location, slot);
		return true;
	}
	else
	{
		return false;
	}
}

GLuint OpenGLRenderer::CreateTexture(const std::string_view& filename)
{
	GLuint textureHandle = 0;

	int32_t textureWidth = 0;
	int32_t textureHeight = 0;
	int32_t bitsPerPixel = 0;

	// Load initialized data into a pointer with stb image loader
	uint8_t* imgdata = stbi_load(filename.data(), &textureWidth, &textureHeight, &bitsPerPixel, STBI_rgb_alpha);
	if (!imgdata || !textureWidth || !textureHeight || !bitsPerPixel)
	{
		IApplication::Debug("Failed to load image");
		IApplication::Debug(filename.data());
		return 0;
	}

	// Multiply color values of all pixels with alpha when loading the data for faster blending
	const int32_t imgdatabytes = textureWidth * textureHeight * 4;
	for (int32_t i = 0; i < imgdatabytes; i += 4) // Loop through pixels
	{
		const int32_t alpha = imgdata[i + 3]; // Fourth byte
		if (alpha != 255)
		{
			// Use fixed point arithmetics for faster computing
			imgdata[i] = imgdata[i] * alpha / 255; // Red
			imgdata[i + 1] = imgdata[i + 1] * alpha / 255; // Green
			imgdata[i + 2] = imgdata[i + 2] * alpha / 255; // Blue
		}
	}

	GLint internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	// Store possible error into a variable for debugging purposes
	GLenum err = glGetError();

	glGenTextures(1, &textureHandle);

	// Activate first texture slot
	glActiveTexture(GL_TEXTURE0);
	// Make texture operations apply to current texture
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	// Transfer image data to GPU memory
	glTexImage2D(GL_TEXTURE_2D,
		0,						// Texture level
		internalFormat,			// Format for pixels in GPU memory
		textureWidth,
		textureHeight,
		0,						// Border
		format,					// Format of incoming data
		GL_UNSIGNED_BYTE,		// Type of the incoming data
		imgdata);

	delete[] imgdata;

	err = glGetError();

	// Set default values for texture filtering and wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Smaller than original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Bigger than original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Texture coordinate x
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Texture coordinate y

	return textureHandle;
}

GLuint OpenGLRenderer::CreateVertexShader(const char* vertexShader)
{
	// Create the vertex shader object
	GLuint shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	// Load source code to the empty shader object
	glShaderSource(shaderHandle, 1, (const char**)&vertexShader, nullptr);
	// Compile the source code
	glCompileShader(shaderHandle);

	// Check if compilation was succesful by getting compile status
	GLint shaderCompiled = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);
	if (!shaderCompiled)
	{
		IApplication::Debug("Failed to compile vertex shader:");
		PrintShaderError(shaderHandle);

		glDeleteShader(shaderHandle);
		shaderHandle = 0;
	}
	return shaderHandle;
}

GLuint OpenGLRenderer::CreateVertexShaderFromFile(const std::string_view& filename)
{
	// Open file with binary mode
	std::ifstream f(filename.data(), std::ios::binary);
	// Read bytes from the file to vector of characters
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(f)),
		(std::istreambuf_iterator<char>()));
	// Add terminating zero for charbuffer incase file does not contain it
	bytes.push_back(0);

	return CreateVertexShader(bytes.data());
}

GLuint OpenGLRenderer::CreateFragmentShader(const char* fragmentShader)
{
	// Create the fragment shader object
	GLuint shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	// Load the source code to the empty shader object
	glShaderSource(shaderHandle, 1, (const char**)&fragmentShader, nullptr);
	// Compile the source code
	glCompileShader(shaderHandle);

	// Check if compilation succeeded
	GLint shaderCompiled;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);
	if (!shaderCompiled)
	{
		IApplication::Debug("Failed to compile fragment shader:");
		PrintShaderError(shaderHandle);

		glDeleteShader(shaderHandle);
		shaderHandle = 0;
	}

	return shaderHandle;
}

GLuint OpenGLRenderer::CreateFragmentShaderFromFile(const std::string_view& filename)
{
	// Load contents of the file to array of chars
	std::ifstream f(filename.data(), std::ios::binary);
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(f)),
		(std::istreambuf_iterator<char>()));
	// Add terminating zero for charbuffer incase file does not contain it
	bytes.push_back(0);

	return CreateFragmentShader(bytes.data());
}

GLuint OpenGLRenderer::CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// Create the shader program
	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, fragmentShader);
	glAttachShader(programHandle, vertexShader);
	glLinkProgram(programHandle);

	GLint linked = 0;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		IApplication::Debug("Failed to link program:");
		PrintProgramError(programHandle);

		glDeleteProgram(programHandle);
		programHandle = 0;
	}

	return programHandle;
}

void OpenGLRenderer::PrintShaderError(GLuint shader)
{
	GLint infologLength = 0;
	GLint charsWritten = 0;
	// Check if there are any log messages for the shader
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength)
	{
		// Allecate enough characters for the log
		char* infoLog = new char[infologLength + 1];
		// Make sure all the bytes are zeros at this point
		memset(infoLog, 0, infologLength + 1);
		// Copy the characters from the log to infoLog pointer
		// Last character will be the terminator zero (just in case)
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		IApplication::Debug(infoLog);
		delete[] infoLog;
	}
}

void OpenGLRenderer::PrintProgramError(GLuint program)
{
	GLint infologLength = 0;
	GLint charsWritten = 0;
	// Check if there are any log messages for the program
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength)
	{
		// Allecate enough characters for the log
		char* infoLog = new char[infologLength + 1];
		// Make sure all the bytes are zeros at this point
		memset(infoLog, 0, infologLength + 1);
		// Copy the characters from the log to infoLog pointer
		// Last character will be the terminator zero (just in case)
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		IApplication::Debug(infoLog);
		delete[] infoLog;
	}
}

bool OpenGLRenderer::InitFunctions()
{
#if defined (_WINDOWS)
#define GL_GETPROCADDRESS wglGetProcAddress
#define GL_GETPROCADDRESS_PARAM_TYPE const char*
#endif
#if defined (_LINUX)
#define GL_GETPROCADDRESS glXGetProcAddressARB
#define GL_GETPROCADDRESS_PARAM_TYPE const GLubyte*
#endif

#if defined (_WINDOWS)
	glBlendEquation = (PFNGLBLENDEQUATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendEquation");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glActiveTexture");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2D)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCompressedTexImage2D");
	glBlendColor = (PFNGLBLENDCOLORPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendColor");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
#endif

	glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendEquationSeparate");
	glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendFuncSeparate");

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetProgramInfoLog");

	glUniform2f = (PFNGLUNIFORM2FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4f");

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteBuffers");

	glBindBuffer = (PFNGLBINDBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindBuffer");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetActiveUniform");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetShaderiv");

	// VBO

	glGenBuffers = (PFNGLGENBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenBuffers");
	glBufferData = (PFNGLBUFFERDATAPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttribPointer");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttribIPointer");


	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenFramebuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenRenderbuffers");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteFramebuffers");
	glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteRenderbuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindFramebuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindRenderbuffer");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glRenderbufferStorage");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferTexture2D");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferTexture");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferRenderbuffer");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCheckFramebufferStatus");
	glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glClearDepthf");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenerateMipmap");

	// Check that functions were loaded properly
	if (!glCreateProgram)
	{
		IApplication::Debug("Renderer_OpenGL::InitFunctions - failed to find required OpenGL functions. Most likely there is no valid OpenGL drivers installed");
		return false;
	}

	return true;
}

bool OpenGLRenderer::SetDefaultSettings()
{
#ifdef _DEBUG
	GLenum err = glGetError();
#endif

	// default states
	glStencilMask(0);
	glDisable(GL_SCISSOR_TEST);

	// Initialize source of geometry data to zero
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Set how data on buffers should be aligned so that textures are shown correctly
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_DEPTH_TEST); // Enable depthbuffer writing
	glEnable(GL_DEPTH_FUNC);
	glDepthMask(GL_TRUE); // Use masking
	glDepthFunc(GL_LEQUAL); // Accept pixel to framebuffer if depth value is less or equal than the value that was previously in depthbuffer
	glDisable(GL_STENCIL_TEST); // Disable stencilbuffer

	glClearDepthf(1.0f); // Set furthest away value for clearing depthbuffer
	glClearStencil(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Specify pixel colors for clearing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear all buffers to get fresh start
	glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Do not render pixels that are facing away from the camera
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_CCW);

	return true;
}