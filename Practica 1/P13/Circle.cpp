#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include "basewin.h"
#include <WindowsX.h>

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
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
		return D2D1::Point2F(static_cast<float>(x) / scaleX,
			static_cast<float>(y) / scaleY);
	}
};
float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

class MainWindow : public BaseWindow<MainWindow>
{
	ID2D1Factory *pFactory;
	ID2D1HwndRenderTarget *pRenderTarget;
	ID2D1SolidColorBrush *pBrush;
	ID2D1SolidColorBrush *pStroke;
	ID2D1SolidColorBrush *pFill;
	UINT_PTR Timer;
	float fHourAngle;
	float fSecondsAngle;
	float fMinuteAngle;
	D2D1_POINT_2F ptMouse;



	D2D1_ELLIPSE clock;

	D2D1_ELLIPSE ellipse;

	void CalculateLayout();
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();
	void OnPaint();
	void Resize();
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);
public:
	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pFill(NULL),
		ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0)),
		ptMouse(D2D1::Point2F())
	{
	}
	PCWSTR ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth);
	void RenderScene();
	void OnTime();
};
// Recalculate drawing layout when the size of the window changes.
void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 8;
		const float y = size.height / 8;
		const float radius = min(x, y);
		clock = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}
HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);
		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			const D2D1_COLOR_F color1 = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color1, &pFill);

			const D2D1_COLOR_F color2 = D2D1::ColorF(0.0f, 0.0f, 0.0f);
			hr = pRenderTarget->CreateSolidColorBrush(color2, &pStroke);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}
void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}
void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		/*pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		pRenderTarget->FillEllipse(ellipse, pBrush);*/

		RenderScene();
		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		
		EndPaint(m_hwnd, &ps);
	}
}
void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}
void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	SetCapture(m_hwnd);
	ellipse.point = ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	ellipse.radiusX = ellipse.radiusY = 1.0f;
	InvalidateRect(m_hwnd, NULL, FALSE);
}
void MainWindow::OnLButtonUp()
{
	ReleaseCapture();
}
void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	if (flags & MK_LBUTTON)
	{
		const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
		const float width = (dips.x - ptMouse.x) / 2;
		const float height = (dips.y - ptMouse.y) / 2;
		const float x1 = ptMouse.x + width;
		const float y1 = ptMouse.y + height;
		ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		InvalidateRect(m_hwnd, NULL, FALSE);
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
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1; // Fail CreateWindowEx.
		}
		Timer = SetTimer(m_hwnd, 0, 1000, (TIMERPROC)NULL);
		DPIScale::Initialize(pFactory);
		return 0;
	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		KillTimer(m_hwnd, Timer);
		PostQuitMessage(0);

		return 0;
	case WM_PAINT:
		OnPaint();
		return 0;
	case WM_SIZE:
		Resize();
		return 0;
	case WM_TIMER:
		OnTime();
		PostMessage(m_hwnd, WM_PAINT, NULL, NULL);
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
void MainWindow::DrawClockHand(float fHandLength, float fAngle, float
	fStrokeWidth)
{
	pRenderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(fAngle, clock.point)
	);
	// endPoint defines one end of the hand.
	D2D_POINT_2F endPoint = D2D1::Point2F(
		clock.point.x,
		clock.point.y - (clock.radiusY * fHandLength)
	);
	// Draw a line from the center of the ellipse to endPoint.
	pRenderTarget->DrawLine(
		clock.point, endPoint, pStroke, fStrokeWidth);
}
void MainWindow::RenderScene()
{
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
	pRenderTarget->FillEllipse(clock, pFill);
	pRenderTarget->DrawEllipse(clock, pStroke);

	pRenderTarget->FillEllipse(ellipse, pBrush);
	// Draw hands
	
	DrawClockHand(0.6f, fHourAngle, 6);
	DrawClockHand(0.85f, fMinuteAngle, 4);
	DrawClockHand(0.9f, fSecondsAngle, 2);

	
	// Restore the identity transformation.
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
void MainWindow::OnTime() {
	SYSTEMTIME time;

	// 60 minutes = 30 degrees, 1 minute = 0.5 degree
	GetLocalTime(&time);

	 fHourAngle = (360.0f / 12) * (time.wHour) +
		(time.wMinute * 0.5f);
	 fMinuteAngle = (360.0f / 60) * (time.wMinute);
	 fSecondsAngle = (360.0f / 60) * (time.wSecond);
	 
}
