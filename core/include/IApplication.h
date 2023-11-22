#pragma once

#if defined (_WINDOWS)
// Include minimum set of win32 stuff
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined (_LINUX)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#endif


#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iterator>
#include <string_view>

// Include timer component and renderer interface
#include "Timer.h"
#include "IRenderer.h"

// Define some common keycodes
#if defined (_WINDOWS)
#define KEY_ESC		VK_ESCAPE
#define KEY_LEFT	VK_LEFT
#define KEY_RIGHT	VK_RIGHT
#define KEY_UP		VK_UP
#define KEY_DOWN	VK_DOWN
#define KEY_SPACE	VK_SPACE

#elif defined (_LINUX)
#define KEY_ESC		XK_Escape
#define KEY_LEFT	XK_Left
#define KEY_RIGHT	XK_Right
#define KEY_UP		XK_Up
#define KEY_DOWN	XK_Down
#define KEY_SPACE	XK_Space

#endif

// Define an interface for Application
class IApplication
{
public:
	IApplication();
	~IApplication();

	/**
	 * Initialize the application layer
	 * @param resX horizontal resolution of the screen in pixels
	 * @param resY vertical resolution of the screen in pixels
	 * @param title window title text
	 * @return true if successful, false otherwise
	 */
	bool Create(int32_t resX, int32_t resY, const std::string& title);

	/**
	 * Enter into main loop. Returns when app is terminated
	 */
	void Run();

	/**
	 * Pure virtual app initializer.
	 * If implementation returns false, app startup is canceled and app is closed.
	 * @return true if successful, false otherwise.
	 */
	virtual bool OnCreate() = 0;

	/**
	 * Pure virtual app destroyer.
	 * Called by the application when app is about to get destroyed
	 */
	virtual void OnDestroy() = 0;

	/**
	 * Pure virtual app update loop
	 * @param frametime time since previous update, in seconds
	 */
	virtual void OnUpdate(float frametime) = 0;

	/**
	 * Pure virtual app drawing method
	 * @param renderer renderer object
	 */
	virtual void OnDraw(IRenderer& renderer) = 0;

	/**
	 * Check if app is active
	 * @return true if app is active, false otherwise
	 */
	inline bool IsActive() const { return m_bActive; }

	/**
	 * Set the app active when on the foreground or inactive 
	 * when app is on the background.
	 * @param true to set app active, false otherwise
	 */
	void SetActive(bool set);

	/**
	 * Close the app.
	 */
	void Close();

	/**
	 * Get frame time 
	 * @return time since previous update, in seconds
	 */
	inline float GetFrameTime() const { return m_Timer.GetElapsedSeconds(); }

	/**
	 * Get the renderer currently in use.
	 * Do not store the return value of this function!
	 * Only use it locally on a function you call it with
	 * @return pointer to current renderer
	 */
	inline IRenderer* GetRenderer() { return m_pRenderer.get(); }

	/**
	 * Initialize random seed with tick count
	 */
	static void RandSeed();

	/**
	 * Get screen width
	 * @return screen width in pixels
	 */
	inline int32_t GetWidth() const { return m_iWidth; }

	/**
	 * Get screen height
	 * @return screen height in pixels
	 */
	inline int32_t GetHeight() const { return m_iHeight; }

	/**
	 * Get screen's aspect ratio
	 * @return screen aspect ratio
	 */
	inline float GetAspect() const { return (float)m_iWidth / (float)m_iHeight; }

	/**
	 * Check if key is pressed
	 * @param osKeyCode os-dependent key code
	 * @return true if key is down
	 */
	static bool IsKeyDown(uint32_t osKeyCode);

	/**
	 * Get window handle
	 * @return window handle
	 */
#if defined (_WINDOWS)
	inline HWND GetWindow() { return m_Window; }
#endif
#if defined (_LINUX)
	inline Display* GetDisplay() { return m_pDisplay; }
	inline Window GetWindow() { return m_Window; }
#endif


	/**
	 * Static app pointer accessor
	 * @return pointer to active app, or nullptr if app has been terminated
	 */
	static IApplication* GetApp() { return m_pApp; }

	/**
	 * Print debug streams
	 * @param msg message to print to debug stream
	 */
	static void Debug(const wchar_t* msg);
	static void Debug(const std::string& msg);
	static void Debug(const char* msg);

protected:
	/**
	 * Event when screen size changes
	 * @param widthPixels, heightPixels new pixel size of the screen
	 */
	virtual void OnScreenSizeChanged(uint32_t widthPixels, uint32_t heightPixels) {}

	/**
	 * Event when mouse button is pressed down
	 * @param buttonIndex index of the mouse button (0 based)
	 * @param point event coordinate in pixels
	 * @return true if event was handled by the receiver
	 */
	virtual bool OnMouseBegin(int32_t buttonIndex, const glm::vec2& point) { return false; }

	/**
	 * Event when mouse is dragged
	 * @param buttonIndex index of the mouse button (0 based)
	 * @param point event coordinate in pixels
	 * @return true if event was handled by the receiver
	 */
	virtual bool OnMouseDrag(int32_t buttonIndex, const glm::vec2& point) { return false; }

	/**
	 * Event when mouse button is lifted up
	 * @param buttonIndex index of the mouse button (0 based)
	 * @param point event coordinate in pixels
	 * @return true if event was handled by the receiver
	 */
	virtual bool OnMouseEnd(int32_t buttonIndex, const glm::vec2& point) { return false; }

	/**
	 * Key down event
	 * @param keyCode key code
	 * @return true if event was handled by the receiver
	 */
	virtual bool OnKeyDown(uint32_t keyCode) { return false; }

#if defined (_WINDOWS)
	/**
	 * Allow capturing all win32 events before they are handled by default handler.
	 * @param message message id
	 * @param wParam win32 wparam
	 * @param lParam win32 lparam
	 * @return true if event was handled by the receiver, if receiver returns false, event is passed to default handler.
	 */
	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam);
#endif

private:
#if defined (_WINDOWS)
	static HWND MakeWindow(int32_t width, int32_t height, const std::string& title);
	static long WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Handle to window
	HWND							m_Window;
#endif

#if defined (_LINUX)
	static Window MakeWindow(int32_t width, int32_t height, const char* title);
	static bool ProcessEvents(XEvent* evnt);
	static KeySym PreprocessKeyEvent(XEvent* evnt);

	static Display* m_pDisplay;
	static int32_t					m_iScreen;
	static Window					m_Window;
	static XSetWindowAttributes		m_WindowAttributes;
	static Atom						m_CloseAtom;
	static uint8_t                  m_uKeyboard[65536];
#endif
	// Pointer to the class itself so static functions can access it
	static IApplication*			m_pApp;
	// Flag for checking if window is on the foreground or minimized
	bool							m_bActive;

	Timer							m_Timer;

	int32_t							m_iWidth;
	int32_t							m_iHeight;

	std::unique_ptr<IRenderer>		m_pRenderer;
};
