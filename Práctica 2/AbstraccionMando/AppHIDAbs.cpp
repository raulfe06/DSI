#include "HIDXBox.h"
#include <math.h>

HWND hWndAnt;
HWND hWnd;

void GeneraEfectos(HIDXBox* Control);
VOID CALLBACK TimerCallBack();

#define T 10
#define TARGET_XBOX360
#ifdef TARGET_XBOX360
HIDXBox Control(T);
#elif defined (TARGET_PS3)
HIDPs Control(T);
#elif defined (TARGET_WII)
HIDWii Control(T);
#endif


int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	Control.Calibra();
	UINT_PTR ptimerxbox;

	ptimerxbox = SetTimer(NULL, NULL, T, (TIMERPROC)TimerCallBack);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	KillTimer(NULL, ptimerxbox);
	return 0;
}


VOID CALLBACK TimerCallBack()
{
	Control.Actualiza();
	GeneraEfectos(&Control);
}

void GeneraEfectos(HIDXBox* Control)
{


	POINT pt, pt0;
	GetCursorPos(&pt);
	WORD lastButtons = Control->getLast();
	WORD wButtons = Control->getButtons();
	// Sin filtrar
	if (Control->gLJX() != 0 | Control->gLJY() != 0)
	{
		pt.x += 10 * Control->gLJX();
		pt.y -= 10 * Control->gLJY();
		SetCursorPos(pt.x, pt.y);
	}

	RECT Rect = { 0, 0, 1024, 600 };
	HWND Desk = GetDesktopWindow();
	GetWindowRect(Desk, &Rect);

	 if ((fabs(Control->gRJXf()) > 0.01) | (fabs(Control->gRJYf()) > 0.01))
	{
		 pt.x += 20 * Control->gRJXf();
		 pt.y -= 20 * Control->gRJYf();
	//	pt.x = Rect.right / 2 + Rect.right *3 / 4 * Control->gRJXf();
		//pt.y = Rect.bottom / 2 - Rect.bottom  *3 / 4 * Control->gRJYf();
		SetCursorPos(pt.x, pt.y);
	}


	if (Control->BD(XINPUT_GAMEPAD_LEFT_SHOULDER) )
		mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, NULL);
	if (Control->BU(XINPUT_GAMEPAD_LEFT_SHOULDER))
		mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);

	if (Control->BD(XINPUT_GAMEPAD_X)) 
		keybd_event(VK_HOME, 0x24, KEYEVENTF_EXTENDEDKEY | 0, 0);

	if (Control->BD(XINPUT_GAMEPAD_B))
		keybd_event(VK_END, 0x23, KEYEVENTF_EXTENDEDKEY | 0, 0);

	if (Control->BD(XINPUT_GAMEPAD_Y))
		keybd_event(VK_PRIOR, 0x21, KEYEVENTF_EXTENDEDKEY | 0, 0);

	if (Control->BD(XINPUT_GAMEPAD_A))
		keybd_event(VK_NEXT, 0x22, KEYEVENTF_EXTENDEDKEY | 0, 0);


	if (Control->BD(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, NULL);
	if (Control->BU(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);

	if (Control->gLT() != 0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, (DWORD)(-40 * Control->gLT()), 0);
	if (Control->gRT() != 0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, (DWORD)(40 * Control->gRT()), 0);

	if (Control->BD(XINPUT_GAMEPAD_BACK))
		PostQuitMessage(0);

	if (Control->BD(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x0, KEYEVENTF_KEYUP, 0);

	if (Control->BD(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x0, KEYEVENTF_KEYUP, 0);

	if (Control->BD(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x0, KEYEVENTF_KEYUP, 0);

	if (Control->BD(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x0, KEYEVENTF_KEYUP, 0);

	if (Control->BU(XINPUT_GAMEPAD_START))
		keybd_event(VK_RETURN, 0x0D, KEYEVENTF_EXTENDEDKEY | 0, 0);

	if (Control->BD(0xFFFF))
		Control->sRR(1, 0.1);

//Cambio de ventana cuando rota
	hWndAnt = hWnd;
	hWnd = GetForegroundWindow();
	if (Control->GRLJ())
		ShowWindow(hWnd, SW_MINIMIZE);
	if (hWnd != hWndAnt)
		Control->sLR(1, 0.2);
}