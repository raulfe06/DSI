//#include <windows.h>
#include "basewin.h"
#include "Scene.h"
#include <WindowsX.h>

class MainWindow : public BaseWindow<MainWindow>
{
	Scene   Escena;			//Objeto que gestiona la Escena
	RECT	AreaCliente;	//Area Cliente
	UINT_PTR Timer;			//Timer

	int   OnCreate();
	void    OnDestroy();
	void    OnPaint();
	void    OnResize();
	void    OnTime();

public:
	MainWindow() 	{	}
	PCWSTR  ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};



int MainWindow::OnCreate()
{
	GetClientRect(m_hwnd, &AreaCliente);
	if (FAILED(Escena.CreaFactoria()))
	{
		return -1;  // Fail CreateWindowEx.
	}
	//Establezcpo un Timer T=1s; 
	Timer = SetTimer(m_hwnd, 0, 1000, (TIMERPROC)NULL);
	DPIScale::Initialize(Escena.pFactory);
	return 0;
}

void MainWindow::OnDestroy()
{
	Escena.DiscardGraphicsResources();
	Escena.LiberaFactoria();
	KillTimer(m_hwnd, Timer);
	PostQuitMessage(0);
}

void MainWindow::OnPaint()
{
	if (SUCCEEDED(Escena.CreateGraphicsResources(m_hwnd, AreaCliente)))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
			Escena.RenderScene();
		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::OnResize()
{
	if (Escena.pRenderTarget != NULL)
	{
		GetClientRect(m_hwnd, &AreaCliente);
			Escena.CalculateLayout(AreaCliente);
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::OnTime()
{
	SYSTEMTIME	time;
	GetLocalTime(&time);		
	//Actualizo el tiempo de la escena
	Escena.Actualizar(time.wHour,time.wMinute, time.wSecond);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;
	if (!win.Create(L"Circle", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	ShowWindow(win.Window(), nCmdShow);
	// Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return OnCreate();
		
	case WM_TIMER:
		// process the 1-second timer 
		OnTime();
		PostMessage(m_hwnd, WM_PAINT, NULL, NULL);
		return 0;

	case WM_DESTROY:
		OnDestroy();
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;
		
	case WM_SIZE:
		OnResize();
		return 0;
	case WM_LBUTTONDOWN:
		SetCapture(m_hwnd);
		Escena.OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		InvalidateRect(m_hwnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONUP:
		Escena.OnLButtonUp();
		return 0;
	case WM_MOUSEMOVE:
		Escena.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}