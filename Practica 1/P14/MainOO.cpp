#include <windows.h>
#include "basewin.h"
#include "Scene.h"
#include "WindowsX.h"
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <math.h>

enum class ClockMode { RunMode, StopMode };
enum class EditionMode { selectMode, drawMode, dragMode };

class DPIScale
{
	static float scaleX;
	static float scaleY;
public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}
	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};
float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

class MainWindow : public BaseWindow<MainWindow>
{
	Scene   Escena;   //Objeto que gestiona la Escena
	RECT AreaCliente; //Area Cliente
	UINT_PTR Timer;   //Timer

	int   OnCreate();
	void    OnDestroy();
	void    OnPaint();
	void    OnResize();
	void    OnTime();

public:
	MainWindow() { }
	PCWSTR  ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
	BOOL hitTest(D2D1_ELLIPSE ellipse, float x, float y);
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void PonCursor(EditionMode mode);
	void KeyDown(WPARAM w);
	void colorSelector();

	ClockMode runMode = ClockMode::RunMode;
	EditionMode mode = EditionMode::selectMode;
	HCURSOR hCursor;
	PWSTR cursor = IDC_ARROW;

private:
	D2D1_POINT_2F ptMouse;
};




int MainWindow::OnCreate()
{
	GetClientRect(m_hwnd, &AreaCliente);
	hCursor = LoadCursor(NULL, cursor);
	SetCursor(hCursor);
	if (FAILED(Escena.CreaFactoria()))
	{
		return -1;  // Fail CreateWindowEx.
	}
	//Establezcpo un Timer T=1s; 
	Timer = SetTimer(m_hwnd, 0, 0, (TIMERPROC)NULL);
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
	SYSTEMTIME time;
	GetLocalTime(&time);
	//Actualizo el tiempo de la escena
	if (runMode == ClockMode::RunMode) {
		Escena.Actualizar(time.wHour, time.wMinute, time.wSecond);
	}

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
	wchar_t msg[32];
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
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_RBUTTONDOWN:
		if (cursor == IDC_HAND)colorSelector();
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_SYSKEYDOWN:
		OutputDebugString(msg);
		return 0;
	case WM_SYSCHAR:
		OutputDebugString(msg);
		break;
	case WM_SYSKEYUP:
		OutputDebugString(msg);
		break;
	case WM_KEYDOWN:
		KeyDown(wParam);
		return 0;
		OutputDebugString(msg);
		break;
	case WM_KEYUP:
		OutputDebugString(msg);
		break;
	case WM_CHAR:
		OutputDebugString(msg);
		break;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{

	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

	if (hitTest(Escena.ellipse, dips.x, dips.y))
	{
		cursor = IDC_HAND;
	}
	else if (!flags) cursor = IDC_ARROW;
	hCursor = LoadCursor(NULL, cursor);
	SetCursor(hCursor);

	if (flags & MK_LBUTTON)
	{

		const float width = (dips.x - ptMouse.x) / 2;
		const float height = (dips.y - ptMouse.y) / 2;
		const float x1 = ptMouse.x + width;
		const float y1 = ptMouse.y + height;
		switch (mode)
		{
		case EditionMode::drawMode:
			Escena.ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
			InvalidateRect(m_hwnd, NULL, FALSE);
			PonCursor(mode);
			hCursor = LoadCursor(NULL, cursor);
			SetCursor(hCursor);
			break;
		case EditionMode::dragMode:
			Escena.ellipse.point.x = dips.x;
			Escena.ellipse.point.y = dips.y;
			hCursor = LoadCursor(NULL, cursor);
			SetCursor(hCursor);
			break;
		default:
			break;
		}

	}
}

void  MainWindow::OnLButtonUp()
{
	mode = EditionMode::selectMode;
	PonCursor(mode);
	hCursor = LoadCursor(NULL, cursor);
	SetCursor(hCursor);
	ReleaseCapture();
}


BOOL MainWindow::hitTest(D2D1_ELLIPSE ellipse, float x, float y)
{
	const float a = ellipse.radiusX;
	const float b = ellipse.radiusY;
	const float x1 = x - ellipse.point.x;
	const float y1 = y - ellipse.point.y;
	const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b *b));
	return d <= 1.0f;
}

void  MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{

	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

	switch (mode)
	{
	case EditionMode::selectMode:
	{
		if (hitTest(Escena.ellipse, (float)dips.x, (float)dips.y))
		{
			mode = EditionMode::dragMode;
			PonCursor(EditionMode::dragMode);
			ptMouse = dips;
		}
		else
		{
			mode = EditionMode::drawMode;
			PonCursor(EditionMode::drawMode);
			ptMouse = dips;
			SetCapture(m_hwnd);
			Escena.ellipse.point = ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
			Escena.ellipse.radiusX = Escena.ellipse.radiusY = 1.0f;
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
	}
	break;
	case EditionMode::drawMode:
	{
		SetCapture(m_hwnd);
		Escena.ellipse.point = ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
		Escena.ellipse.radiusX = Escena.ellipse.radiusY = 1.0f;
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	break;
	case EditionMode::dragMode:
		ptMouse = dips;
		break;
	}

}

void MainWindow::PonCursor(EditionMode mode)
{
	switch (mode)
	{
	case EditionMode::selectMode:
		cursor = IDC_ARROW;
		break;
	case EditionMode::drawMode:
		cursor = IDC_CROSS;
		break;
	case EditionMode::dragMode:
		cursor = IDC_HAND;
		break;
	default:
		cursor = IDC_ARROW;
		break;
	}
}

void MainWindow::KeyDown(WPARAM w)
{
	switch (w)
	{
	default:
		break;
	case ' ':
		if (runMode == ClockMode::RunMode) { runMode = ClockMode::StopMode; }
		else { runMode = ClockMode::RunMode; }
		break;
	case VK_ESCAPE:
		OnDestroy();
		break;
	}
}

void MainWindow::colorSelector()
{
	CHOOSECOLOR cc; // common dialog box structure
	static COLORREF acrCustClr[16]; // array of custom colors
	static DWORD rgbCurrent; // initial color selection
							 // Initialize CHOOSECOLOR
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		D2D1_COLOR_F color;
		color.r = (float)GetRValue(cc.rgbResult) / 255;
		color.g = (float)GetGValue(cc.rgbResult) / 255;
		color.b = (float)GetBValue(cc.rgbResult) / 255;
		color.a = 1.0f;

		Escena.pEllipse->SetColor(color);
	}
}