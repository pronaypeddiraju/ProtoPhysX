//------------------------------------------------------------------------------------------------------------------------------ 
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
//Engine Systems
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/WindowContext.hpp"
//Game Systems
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

//Purely for debugging
#include <stdio.h>

extern App* g_theApp;

HWND g_hWnd = nullptr;							
HDC g_displayDeviceContext = nullptr;			
HGLRC g_openGLRenderingContext = nullptr;		
WindowContext* g_windowContext = nullptr;
const char* APP_NAME = "Protogame 3D";	

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
static bool AppWindowProc( void* windowHandle, uint32_t wmMessageCode, uintptr_t wParam, uintptr_t lParam )
{
	UNREFERENCED_PARAMETER(windowHandle); 
	UNREFERENCED_PARAMETER(wParam); 
	UNREFERENCED_PARAMETER(lParam); 

	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		g_theApp->HandleQuitRequested();
		return true;
	}

	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char) wParam;
		if(g_theApp->HandleKeyPressed(asKey))
		{
			return 0;
		}
		if( asKey == VK_ESCAPE )
		{
			g_theApp->HandleQuitRequested();
			return true; 
		}


		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char) wParam;
		if(g_theApp->HandleKeyReleased(asKey))
		{
			return true; 
		}

		break;
	}

	case WM_CHAR:
	{
		unsigned char asKey = (unsigned char) wParam;

		bool is_released = ((lParam & (1U << 31)) != 0);
		if((asKey >= 20 && asKey <= 126 && !is_released))
		{
			//Ignore escape character and tilde
			if(asKey != 96 && asKey != 27)
			{
				g_theApp->HandleCharacter(asKey);
				return 0;
			}
		}
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return false; 
}

//------------------------------------------------------------------------------------------------------------------------------
void CreateWindowAndRenderContext( float clientAspect )
{
	g_windowContext = new WindowContext();
	g_windowContext->Create( APP_NAME, clientAspect, .90f, AppWindowProc ); 

	// this is the end of the windows part
	g_renderContext = new RenderContext(g_windowContext);
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
void RunMessagePump()
{
	g_windowContext->BeginFrame(); 
}

//-----------------------------------------------------------------------------------------------
void Startup()
{
	//We create app first and read black board. Then we create window and we get the window data based on black board info to create either full screen/ windowed screen
	//CreateOpenGLWindow( applicationInstanceHandle, CLIENT_ASPECT );

	//Here call a CreateWindow 
	CreateWindowAndRenderContext( CLIENT_ASPECT );
	g_theApp = new App();	
	g_theApp->StartUp();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;
	g_theApp = nullptr;

	delete g_windowContext; 
	g_windowContext = nullptr; 
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );
	Startup();

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		RunMessagePump();
		g_theApp->RunFrame();
		//SwapBuffers(g_displayDeviceContext);
		Sleep(0);
	}

	Shutdown();
	return 0;
}


