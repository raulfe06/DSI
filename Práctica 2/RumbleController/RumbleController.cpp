//-----------------------------------------------------------------------------
// File: RumbleController.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <XInput.h>
#include <basetsd.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include "resource.h"


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT UpdateControllerState();
void RenderFrame();
UINT_PTR timerMando;
float contadorVibracion;
bool teclaPulsada_ = false;

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define MAX_CONTROLLERS     4
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.


//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STATE
{
	XINPUT_STATE lastState;
	XINPUT_STATE state;
	DWORD dwResult;
	bool bLockVibration;
	XINPUT_VIBRATION vibration;
	bool bConnected;
};

CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
WCHAR g_szMessage[4][1024] = { 0 };
HWND g_hWnd = GetActiveWindow();
bool    g_bDeadZoneOn = true;
POINT pt;



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application.  Since we use a simple dialog for 
//       user interaction we don't need to pump messages.
//-----------------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// Register the window class
	HBRUSH hBrush = CreateSolidBrush(0xFF0000);
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), 0, MsgProc, 0L, 0L, hInst, NULL,
		LoadCursor(NULL, IDC_ARROW), hBrush,
		NULL, L"XInputSample", NULL
	};
	RegisterClassEx(&wc);

	// Create the application's window
	g_hWnd = CreateWindow(L"XInputSample", L"XInput Sample: RumbleController",
		WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
		NULL, NULL, hInst, NULL);

	// Init state
	ZeroMemory(g_Controllers, sizeof(CONTROLER_STATE) * MAX_CONTROLLERS);

	// Enter the message loop
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;

	while (WM_QUIT != msg.message)
	{
		timerMando = SetTimer(g_hWnd, NULL, 10, NULL);
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}

	// Clean up 
	KillTimer(NULL, timerMando);
	UnregisterClass(L"XInputSample", NULL);

	return 0;

}

//-----------------------------------------------------------------------------
HRESULT UpdateControllerState()
{
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		g_Controllers[i].lastState = g_Controllers[i].state;
		g_Controllers[i].dwResult = XInputGetState(i, &g_Controllers[i].state);

		if (g_Controllers[i].dwResult == ERROR_SUCCESS)
			g_Controllers[i].bConnected = true;
		else
			g_Controllers[i].bConnected = false;



	}

	return S_OK;
}


//-----------------------------------------------------------------------------
void RenderFrame()
{
	bool bRepaint = false;
	
	WCHAR sz[4][1024];
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (g_Controllers[i].bConnected)
		{
			
			WORD wButtons = g_Controllers[i].state.Gamepad.wButtons;
			if (g_bDeadZoneOn)
			{
				// Zero value if thumbsticks are within the dead zone 
				if ((g_Controllers[i].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
					g_Controllers[i].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
					(g_Controllers[i].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
						g_Controllers[i].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
				{
					g_Controllers[i].state.Gamepad.sThumbLX = 0;
					g_Controllers[i].state.Gamepad.sThumbLY = 0;
				}

				if ((g_Controllers[i].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
					g_Controllers[i].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
					(g_Controllers[i].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
						g_Controllers[i].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
				{
					g_Controllers[i].state.Gamepad.sThumbRX = 0;
					g_Controllers[i].state.Gamepad.sThumbRY = 0;
				}
			}
			GetCursorPos(&pt);
			if (g_Controllers[i].state.Gamepad.sThumbLX != 0 | (g_Controllers[i].state.Gamepad.sThumbLY != 0))
			{
				pt.x += 10 * (float)g_Controllers[i].state.Gamepad.sThumbLX / (float)MAXINT16;
				pt.y -= 10 * (float)g_Controllers[i].state.Gamepad.sThumbLY / (float)MAXINT16;
				SetCursorPos(pt.x, pt.y);
			}	
			if (g_Controllers[i].state.Gamepad.bLeftTrigger != 0 |(g_Controllers[i].state.Gamepad.bRightTrigger != 0))
			{
				mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, g_Controllers[i].state.Gamepad.bLeftTrigger/4, NULL);
				mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, -g_Controllers[i].state.Gamepad.bRightTrigger/4, NULL);

			}

			DWORD lastButtons = g_Controllers[i].lastState.Gamepad.wButtons;
			if (wButtons & XINPUT_GAMEPAD_LEFT_THUMB && !(lastButtons & XINPUT_GAMEPAD_LEFT_THUMB)) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0,NULL);
			}
			if (!(wButtons & XINPUT_GAMEPAD_LEFT_THUMB) && (lastButtons & XINPUT_GAMEPAD_LEFT_THUMB)) {
				mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);
			}
			if (!(wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && (lastButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {
				keybd_event(VK_LEFT, 0x25, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (!(wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && (lastButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
				keybd_event(VK_RIGHT, 0x27, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (!(wButtons & XINPUT_GAMEPAD_DPAD_UP) && (lastButtons & XINPUT_GAMEPAD_DPAD_UP)) {
				keybd_event(VK_UP, 0x26, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (!(wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && (lastButtons & XINPUT_GAMEPAD_DPAD_DOWN)) {
				keybd_event(VK_DOWN, 0x28, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}			
			if (wButtons & XINPUT_GAMEPAD_RIGHT_THUMB && !(lastButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) {
				mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, NULL);
			}
			if (!(wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) && (lastButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) {
				mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);

			}

			if (wButtons & XINPUT_GAMEPAD_TRIGGER_THRESHOLD && !(lastButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, g_Controllers[0].state.Gamepad.bLeftTrigger, NULL);
			}
			if (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER && !(lastButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, g_Controllers[0].state.Gamepad.bRightTrigger, NULL);
			}


			if (wButtons & XINPUT_GAMEPAD_B && !(lastButtons & XINPUT_GAMEPAD_B)) {
				keybd_event(VK_END, 0x23, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (wButtons & XINPUT_GAMEPAD_X && !(lastButtons & XINPUT_GAMEPAD_X)) {
				keybd_event(VK_HOME, 0x24, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (wButtons & XINPUT_GAMEPAD_Y && !(lastButtons & XINPUT_GAMEPAD_Y)) {
				keybd_event(VK_PRIOR, 0x21, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (wButtons & XINPUT_GAMEPAD_A && !(lastButtons & XINPUT_GAMEPAD_A)) {
				keybd_event(VK_NEXT, 0x22, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			
			if (wButtons & XINPUT_GAMEPAD_BACK && !(lastButtons & XINPUT_GAMEPAD_BACK)) {
				keybd_event(VK_ESCAPE, 0x1B, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}
			if (wButtons & XINPUT_GAMEPAD_START && !(lastButtons & XINPUT_GAMEPAD_START)) {
				keybd_event(VK_RETURN, 0x0D, KEYEVENTF_EXTENDEDKEY | 0, 0);
			}

			if (wButtons & 0xFFFFFFFF) {
				teclaPulsada_ = true;
				g_Controllers[i].bLockVibration = !g_Controllers[i].bLockVibration;
				g_Controllers[i].vibration.wLeftMotorSpeed = 60000;
				
			}
			
			if (!g_Controllers[i].bLockVibration)
			{
				// Map bLeftTrigger's 0-255 to wLeftMotorSpeed's 0-65535
				if (g_Controllers[i].state.Gamepad.bLeftTrigger > 0)
					g_Controllers[i].vibration.wLeftMotorSpeed = ((g_Controllers[i].state.Gamepad.bLeftTrigger +
						1) * 256) - 1;
				else
					g_Controllers[i].vibration.wLeftMotorSpeed = 0;

				// Map bRightTrigger's 0-255 to wRightMotorSpeed's 0-65535
				if (g_Controllers[i].state.Gamepad.bRightTrigger > 0)
					g_Controllers[i].vibration.wRightMotorSpeed = ((g_Controllers[i].state.Gamepad.bRightTrigger +
						1) * 256) - 1;
				else
					g_Controllers[i].vibration.wRightMotorSpeed = 0;
			}

			if ((g_Controllers[i].state.Gamepad.wButtons) &&
				(g_Controllers[i].lastState.Gamepad.wButtons == 0))
			{
				if (!(!g_Controllers[i].bLockVibration && g_Controllers[i].vibration.wRightMotorSpeed == 0 &&
					g_Controllers[i].vibration.wLeftMotorSpeed == 0))
					g_Controllers[i].bLockVibration = !g_Controllers[i].bLockVibration;
			}

			XInputSetState(i, &g_Controllers[i].vibration);
			
			StringCchPrintfW(sz[i], 1024,
				L"Controller %d: Connected\n"
				L"  Buttons: %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
				L"  Left Trigger: %d\n"
				L"  Right Trigger: %d\n"
				L"  Left Thumbstick: %d/%d\n"
				L"  Right Thumbstick: %d/%d", i,
				(wButtons & XINPUT_GAMEPAD_DPAD_UP) ? L"DPAD_UP " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? L"DPAD_DOWN " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? L"DPAD_LEFT " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? L"DPAD_RIGHT " : L"",
				(wButtons & XINPUT_GAMEPAD_START) ? L"START " : L"",
				(wButtons & XINPUT_GAMEPAD_BACK) ? L"BACK " : L"",
				(wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? L"LEFT_THUMB " : L"",
				(wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? L"RIGHT_THUMB " : L"",
				(wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? L"LEFT_SHOULDER " : L"",
				(wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? L"RIGHT_SHOULDER " : L"",
				(wButtons & XINPUT_GAMEPAD_A) ? L"A " : L"",
				(wButtons & XINPUT_GAMEPAD_B) ? L"B " : L"",
				(wButtons & XINPUT_GAMEPAD_X) ? L"X " : L"",
				(wButtons & XINPUT_GAMEPAD_Y) ? L"Y " : L"",
				g_Controllers[i].state.Gamepad.bLeftTrigger,
				g_Controllers[i].state.Gamepad.bRightTrigger,
				g_Controllers[i].state.Gamepad.sThumbLX,
				g_Controllers[i].state.Gamepad.sThumbLY,
				g_Controllers[i].state.Gamepad.sThumbRX,
				g_Controllers[i].state.Gamepad.sThumbRY,
				L"Controller %d: Connected\n"
				L"  Left Motor Speed: %d\n"
				L"  Right Motor Speed: %d\n"
				L"  Rumble Lock: %d\n", i,
				g_Controllers[i].vibration.wLeftMotorSpeed,
				g_Controllers[i].vibration.wRightMotorSpeed,
				g_Controllers[i].bLockVibration);

		}


		else if (g_Controllers[i].dwResult == ERROR_DEVICE_NOT_CONNECTED)
		{
			StringCchPrintf(sz[i], 1024,
				L"Controller %d: Not connected", i);
		}
		else
		{
			StringCchPrintf(sz[i], 1024,
				L"Controller %d: Generic error", i);
		}

		if (wcscmp(sz[i], g_szMessage[i]) != 0)
		{
			StringCchCopy(g_szMessage[i], 1024, sz[i]);
			bRepaint = true;
		}

	}
	if (bRepaint)
	{
		// Repaint the window if needed 
		InvalidateRect(g_hWnd, NULL, TRUE);
		UpdateWindow(g_hWnd);
	}

	// This sample doesn't use Direct3D.  Instead, it just yields CPU time to other 
	// apps but this is not typically done when rendering
	//Sleep(10);
}




//-----------------------------------------------------------------------------
// Window message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HCURSOR hCurs1, hCurs2;    // cursor handles 

	POINT pt;                  // cursor location  
	RECT rc;                   // client area coordinates 
	static int repeat = 1;     // repeat key counter 

	switch (msg)
	{
	case WM_ACTIVATEAPP:
	{
		if (wParam == TRUE)
		{
			// App is now active, so re-enable XInput
			XInputEnable(true);
		}
		else
		{
			// App is now inactive, so disable XInput to prevent
			// user input from effecting application and to 
			// disable rumble. 
			//XInputEnable(false);
		}
		break;
	}
	case WM_KEYDOWN:

		if (wParam == 'D') g_bDeadZoneOn = !g_bDeadZoneOn;
		else if (wParam == VK_ESCAPE) PostQuitMessage(0);
		break;

	case WM_PAINT:
	{
		// Paint some simple explanation text
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		SetBkColor(hDC, 0xFF0000);
		SetTextColor(hDC, 0xFFFFFF);
		RECT rect;
		GetClientRect(hWnd, &rect);

		rect.top = 20;
		rect.left = 20;
		DrawText(hDC,
			L"Use the controller's left/right trigger to adjust the speed of the left/right rumble motor.\n"
			L"Press any controller button to lock or unlock at the current rumble speed.\n",
			-1, &rect, 0);

		 for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
            {
                rect.top = i * 120 + 70;
                rect.left = 20;
                DrawText( hDC, g_szMessage[i], -1, &rect, 0 );
            }

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_TIMER:
	{
		UpdateControllerState();
		RenderFrame();
		if (teclaPulsada_) {
			if (contadorVibracion > 0) {
				contadorVibracion--;
			}
			else {
				contadorVibracion = 10;
				teclaPulsada_ = false;
				g_Controllers[0].bLockVibration = !g_Controllers[0].bLockVibration;
			}
		}
		break;
	}
	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}



