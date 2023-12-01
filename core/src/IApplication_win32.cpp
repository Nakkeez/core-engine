#include "../include/IApplication.h"

// Include all your renderers
#include "../include/OpenGLRenderer.h"

#if defined (_WIN32)

// Declare static member variable outside of the class members
IApplication* IApplication::m_pApp = nullptr;


IApplication::IApplication() :
	m_Window(nullptr),
	m_bActive(false),
	m_iWidth(0),
	m_iHeight(0)
{
	m_pApp = this;
}

IApplication::~IApplication()
{
	m_pApp = nullptr;
}

bool IApplication::Create(int32_t resX, int32_t resY, const std::string& title)
{
	m_Window = MakeWindow(resX, resY, title);
	if (m_Window)
	{
		m_iWidth = resX;
		m_iHeight = resY;

		// Create the renderer
		m_pRenderer = std::make_unique<OpenGLRenderer>();
		// Don't start the app if something went wrong in the graphics initialization
		if (!m_pRenderer->Create())
		{
			return false;
		}

		// Call pure virtual OnCreate
		if (OnCreate())
		{
			SetActive(true);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void IApplication::Run()
{
	// Run the app
	MSG msg;
	BOOL gotMsg = false;

	// Check if Windows has messages for window and copy the newest one
	::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	while (msg.message != WM_QUIT)
	{
		if (IsActive())
		{
			// App is active, use PeekMessage not to block the execution
			// while receiving messages from Windows
			gotMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		}
		else
		{
			// App is not active, use GetMessage which blocks until
			// message is received. This allows OS to use power saving and
			// task switching more efficently when screen update is not needed.
			gotMsg = ::GetMessage(&msg, nullptr, 0, 0);
		}

		if (gotMsg)
		{
			::TranslateMessage(&msg); // Do preprocessing, like user settings, for the message
			::DispatchMessage(&msg); // Call Window Procedure with the event
		}

		if (msg.message != WM_QUIT)
		{
			// Calculate the complete time of previous frame
			m_Timer.EndTimer();
			m_Timer.BeginTimer();

			OnUpdate(m_Timer.GetElapsedSeconds());
			OnDraw(*m_pRenderer); // Use reference of renderer so the parameter will not be null
			m_pRenderer->Flip(); // Display graphics that are produced inside OnDraw
		}
	}

	OnDestroy();
	m_pRenderer = nullptr;
}

void IApplication::SetActive(bool set)
{
	m_bActive = set;
	// Begin timer fresh when app is activated/deactivated to prevent too long frametimes
	m_Timer.BeginTimer();
}

void IApplication::Close()
{
	::PostQuitMessage(0);
}

void IApplication::RandSeed()
{
	// Get random seed with tick count
	srand((uint32_t)::GetTickCount64());
}

bool IApplication::IsKeyDown(uint32_t osKeyCode)
{
	// Check if a key is being pressed
	return ::GetAsyncKeyState(osKeyCode);
}

void IApplication::Debug(const wchar_t* msg)
{
	// Output white debug message
	::OutputDebugStringW(msg);
}


void IApplication::Debug(const std::string& msg)
{
	// Output debug message as raw string data
	::OutputDebugStringA(msg.c_str());
}


void IApplication::Debug(const char* msg)
{
	// Output ANSI version of debug message
	::OutputDebugStringA(msg);
}

bool IApplication::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			// Set app inactive while minimized
			SetActive(false);
		}
		else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
		{
			RECT rect;
			::GetClientRect(GetWindow(), &rect);
			// Calculate new window size
			const int32_t windowWidth = rect.right - rect.left;
			const int32_t windowHeight = rect.bottom - rect.top;

			if (windowWidth != m_iWidth || windowHeight != m_iHeight)
			{
				m_iWidth = windowWidth;
				m_iHeight = windowHeight;
				// Set viewport so that it will fit the entire window after resize
				m_pRenderer->SetViewport({ 0, 0, m_iWidth, m_iHeight });
				OnScreenSizeChanged(m_iWidth, m_iHeight);
			}

			// Set active again when app is restored to the foreground
			SetActive(true);
		}
		break;

	case WM_KEYDOWN:
		OnKeyDown((uint32_t)wParam);
		break;

	case WM_LBUTTONDOWN:
		OnMouseBegin(0, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_MBUTTONDOWN:
		OnMouseBegin(1, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_RBUTTONDOWN:
		OnMouseBegin(2, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_LBUTTONUP:
		OnMouseEnd(0, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_MBUTTONUP:
		OnMouseEnd(1, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_RBUTTONUP:
		OnMouseEnd(2, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MOUSEMOVE:
		int32_t pointerIndex = -1;
		if ((wParam & MK_LBUTTON) != 0) pointerIndex = 0;
		else if ((wParam & MK_MBUTTON) != 0) pointerIndex = 1;
		else if ((wParam & MK_RBUTTON) != 0) pointerIndex = 2;

		if (pointerIndex > -1)
		{
			OnMouseDrag(pointerIndex, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		}
		break;

	}
	return false;
}

HWND IApplication::MakeWindow(int32_t width, int32_t height, const std::string& title)
{
	HINSTANCE hInst = ::GetModuleHandle(nullptr);
	DWORD windowStyle = 0;

	windowStyle = WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_THICKFRAME |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX;

	WNDCLASS wc;
	// Set members of data structure to zero
	memset(&wc, 0, sizeof(WNDCLASS));

	wc.style = CS_HREDRAW | CS_VREDRAW; // Window will be refreshed horizontally and vertically
	wc.lpfnWndProc = (WNDPROC)WndProc; // Window Procedure
	wc.hInstance = hInst;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION); // Use default application icon
	wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW); // Use default application cursor (arrow)
	wc.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH); // Window will be solid black on default
	wc.lpszClassName = L"GRAPHICSENGINE_WNDCLASS"; // Give unique name for window class

	// Check if window class could be registered
	if (!::RegisterClass(&wc))
	{
		Debug("Failed to register window class, exiting...");
		return nullptr;
	}

	// Change 8bit title string into 16bit widestring
	auto widestr = std::wstring(title.begin(), title.end());
	// Create the window
	HWND window = ::CreateWindow(
		wc.lpszClassName, // Tell Windows what type of window to create
		widestr.c_str(),
		windowStyle,
		CW_USEDEFAULT, // X position of window
		CW_USEDEFAULT, // Y position of window
		width,
		height,
		nullptr, // Parent window
		nullptr, // Menu
		hInst, // Instance
		nullptr); // Parameters

	if (!window)
	{
		Debug("Failed to create window, exiting...");
		return nullptr;
	}

	::SetWindowLong(window, GWL_STYLE, windowStyle);
	// Get window area without title bars, borders etc.
	RECT clientArea = { 0, 0, width, height };
	// Modify client area for what it would be for current window coordinates
	::AdjustWindowRectEx(&clientArea, windowStyle, FALSE, 0);
	// Change the window size to client area's width and height
	::SetWindowPos(
		window,
		nullptr,
		0,
		0,
		clientArea.right - clientArea.left,
		clientArea.bottom - clientArea.top,
		SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
	::UpdateWindow(window);
	::ShowWindow(window, SW_SHOWNORMAL);

	return window;
}


long WINAPI IApplication::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		::PostQuitMessage(0); // Send quit message after window is destroyed
		return 0;
		break; // Some compilers may give a warning without this

	case WM_CREATE:
		// Window will pop to the top of window stack when created
		::SetForegroundWindow(hwnd);
		break;

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		return 0;
		break;

	default:
		break;
	}

	bool callDefWndProc = true;
	auto app = IApplication::GetApp();
	if (app)
	{
		// OnEvent returns true if it has handled everything and
		// call to DefWindowProc is not desired
		callDefWndProc = !app->OnEvent(message, wParam, lParam);
	}

	if (callDefWndProc)
	{
		return (long)::DefWindowProc(hwnd, message, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

#endif	// #if defined (_WINDOWS)
