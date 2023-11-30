#include "../include/IApplication.h"

// Include all your renderers
#include "../include/OpenGLRenderer.h"

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

//#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>


#if defined (_LINUX)

IApplication*           IApplication::m_pApp = nullptr;

Display*                 IApplication::m_pDisplay;
int                      IApplication::m_iScreen;
Window                   IApplication::m_Window;
XSetWindowAttributes     IApplication::m_WindowAttributes;
Atom                     IApplication::m_CloseAtom;
uint8_t                  IApplication::m_uKeyboard[65536];


IApplication::IApplication() :
    m_bActive(false)
{
	m_pApp = this;

    m_pDisplay = XOpenDisplay(NULL);
    m_iScreen = DefaultScreen(m_pDisplay);
	memset(m_uKeyboard, 0, 65536);
}


IApplication::~IApplication()
{
	m_pApp = nullptr;
}


bool IApplication::Create(int resX, int resY, const std::string& title)
{
	m_Window = MakeWindow(resX, resY, title.c_str());
	if (m_Window)
	{
		m_iWidth = resX;
		m_iHeight = resY;

		// create the renderer
        m_pRenderer = std::make_unique<OpenGLRenderer>();
		if (!m_pRenderer->Create())
		{
			return false;
		}

		// call pure virtual OnCreate
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


bool IApplication::ProcessEvents(XEvent* evnt)
{
    IApplication* pApp = IApplication::GetApp();
    if (!m_Window || !pApp)
    {
        return false;
    }

    switch (evnt->type)
    {
    case ClientMessage:
        if (m_CloseAtom && (long unsigned int)evnt->xclient.data.l[0] == m_CloseAtom)
        {
            m_Window = 0;
        }
        break;

    case ButtonPress:
        if (evnt->xbutton.button == 4 || evnt->xbutton.button == 5)
        {
            // Mouse wheel event
        }
        else
        {
            // Mouse button event
            const int touchIndex = evnt->xbutton.button - 1;
       		pApp->OnMouseBegin(touchIndex, glm::vec2(evnt->xbutton.x, evnt->xbutton.y));
        }
        break;

    case ButtonRelease:
        if (evnt->xbutton.button < 4)
        {
            const int touchIndex = evnt->xbutton.button - 1;
       		pApp->OnMouseEnd(touchIndex, glm::vec2(evnt->xbutton.x, evnt->xbutton.y));
        }
        break;

    case MotionNotify:
        // First mouse button that is held down
        pApp->OnMouseDrag(0, glm::vec2(evnt->xbutton.x, evnt->xbutton.y));
        break;

    case KeyPress:
        {
            KeySym xkey = PreprocessKeyEvent(evnt);
           	m_uKeyboard[xkey & 0xffff] = 1;

            pApp->OnKeyDown((uint32_t)xkey);
        }
        break;

    case KeyRelease:
        {
            KeySym xkey = PreprocessKeyEvent(evnt);
           	m_uKeyboard[xkey & 0xffff] = 0;
        }
        break;

    case FocusIn:
        pApp->SetActive(true);
        break;

    case FocusOut:
        pApp->SetActive(false);
        break;

    case Expose:
        break;

    case ResizeRequest:
        break;

    case VisibilityNotify:
    {
        XVisibilityEvent* visibility = (XVisibilityEvent*)evnt;
        switch (visibility->state)
        {
        case VisibilityFullyObscured:
            break;

        case VisibilityPartiallyObscured:
            break;

        case VisibilityUnobscured:
            break;

        default:
            break;
        }
    }
    break;

    case ConfigureNotify:
        if (evnt->xconfigure.width != pApp->m_iWidth || evnt->xconfigure.height != pApp->m_iHeight)
        {
            pApp->m_iWidth = evnt->xconfigure.width;
            pApp->m_iHeight = evnt->xconfigure.height;
            pApp->m_pRenderer->SetViewport(0, 0, pApp->m_iWidth, pApp->m_iHeight);
            pApp->OnScreenSizeChanged(pApp->m_iWidth, pApp->m_iHeight);
        }
        break;

    case GraphicsExpose:
        break;

    case NoExpose:
        break;

    default:
        break;
    }

    return true;
}


KeySym IApplication::PreprocessKeyEvent(XEvent* evnt)
{
    KeySym xkey = XLookupKeysym(&evnt->xkey, 0);

    // In Maemo, hardware keyboard enter is XK_KP_Enter, not XK_Return
    // For now, lets map XK_KP_Enter to XK_Return
    if (xkey == XK_KP_Enter)
    {
        xkey = XK_Return;
    }
    else if (xkey >= 97 && xkey <= 122)
    {
        // Convert 'a' to 'z' to 'A' to 'Z'
        xkey -= 32;
    }
    return xkey;
}


void IApplication::SetActive(bool set)
{
	m_bActive = set;
	memset(m_uKeyboard, 0, 65536);

	m_Timer.BeginTimer();
}


void IApplication::Close()
{
	m_Window = 0;
}


void IApplication::Run()
{
    // Run the application
    XEvent event;
    bool bGotMsg = false;

    while (m_Window)
    {
        bGotMsg = false;
        if (XPending(m_pDisplay))
        {
            XNextEvent(m_pDisplay, &event);
            bGotMsg = true;
        }

        if (bGotMsg)
        {
            ProcessEvents(&event);
        }
        else
        {
			m_Timer.EndTimer();
			m_Timer.BeginTimer();

			OnUpdate(m_Timer.GetElapsedSeconds());
			OnDraw(*m_pRenderer);
			m_pRenderer->Flip();
        }
    }
	OnDestroy();
    m_pRenderer = nullptr;
}


Window IApplication::MakeWindow(int width, int height, const char* title)
{
    Window wnd = 0;
    Window rootWindow = DefaultRootWindow(m_pDisplay);

    int i32Depth = XDefaultDepth(m_pDisplay, m_iScreen);
    XVisualInfo* x11Visual = (XVisualInfo*)alloca(sizeof(XVisualInfo));
    Colormap x11Colormap;

    XMatchVisualInfo(m_pDisplay, m_iScreen, i32Depth, TrueColor, x11Visual);
    if (!x11Visual)
    {
        return 0;
    }

    // Colormap of the specified visual type for the display.
    x11Colormap = XCreateColormap(m_pDisplay, rootWindow, x11Visual->visual, AllocNone);
    m_WindowAttributes.colormap = x11Colormap;

    m_WindowAttributes.event_mask =
        ExposureMask | VisibilityChangeMask | KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        StructureNotifyMask | SubstructureNotifyMask | FocusChangeMask |
        PropertyChangeMask | 0;

    wnd = XCreateWindow(m_pDisplay, rootWindow, 0, 0, width, height, 0,
                         CopyFromParent, InputOutput, CopyFromParent,
                         CWBackPixel | CWBorderPixel | CWEventMask |
                             CWColormap, // windowMask,
                         &m_WindowAttributes);

    {
        // create atom for window close button
        m_CloseAtom = XInternAtom(m_pDisplay, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(m_pDisplay, wnd, &m_CloseAtom, 1);

        XSetStandardProperties(m_pDisplay, wnd, title, title, None, NULL, 0, NULL);

        XMapRaised(m_pDisplay, wnd);
        XFlush(m_pDisplay);
    }

    XWMHints* hints = NULL;
    hints = XAllocWMHints();
    hints->input = True;
    hints->flags = InputHint;
    XSetWMHints(m_pDisplay, wnd, hints);
    XFree(hints);

    // Find out what size window we actually got.
    XWindowAttributes xWindowAttributes;
    XGetWindowAttributes(m_pDisplay, wnd, &xWindowAttributes);

    return wnd;
}


void IApplication::Debug(const wchar_t* msg)
{
    fwprintf(stderr, L"%s", msg);
}


void IApplication::Debug(const std::string& msg)
{
    fprintf(stderr, "%s", msg.c_str());
}


void IApplication::Debug(const char* msg)
{
    fprintf(stderr, "%s", msg);
}


void IApplication::RandSeed()
{
	struct timespec ts;
	srand(clock_gettime(CLOCK_MONOTONIC, &ts));
}


bool IApplication::IsKeyDown(uint32_t osKeyCode)
{
    return m_uKeyboard[osKeyCode & 0xffff];
}

#endif	// #if defined (_LINUX)
