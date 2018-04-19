#pragma once/*donde se hace la ventana y esas movidas...*/
#include"HIDXbox.h"

#define T 10 //ms para actualizar
#define TARGET_XBOX360
#ifdef TARGET_XBOX360
HIDXbox Control(T);
#elif defined(TARGET_PS3)
HIDPs Control(T);
#elif defined(TARGET_WII)
HIDWii Control(T);
#endif

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	//Control.calibra();
	UINT_PTR ptTimerxbox;
	// Register the window class
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

VOID CALLBACK TimerCallBack()
{
	Control.Actualiza(); //Actualiza nuestro HID
	//GeneraEfectos(&Control);//Genera los efectos en la aplicación en función de los gestos del control
}

Genraefectos
point pt, pt0;
getcursorpos(&pt);
if(fabs(Control->glIx())>0.05) | fabs (control->glIY)