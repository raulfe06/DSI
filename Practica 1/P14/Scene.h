#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma once
#include "Modelo.h"



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



class Scene: public Modelo
{
public:
	Scene();
	~Scene();
	ID2D1Factory            *pFactory;
	ID2D1SolidColorBrush    *pBrush; //color de Esfera
	ID2D1SolidColorBrush    *pStroke; //color de flechas
	ID2D1HwndRenderTarget   *pRenderTarget;
	D2D1_ELLIPSE             ellipse;
	D2D1_POINT_2F ptMouse;
	

	//Manejar recursos gráficos
	HRESULT CreaFactoria();
	void	LiberaFactoria();
	HRESULT CreateGraphicsResources(HWND m_hwnd,RECT rc);
	void    DiscardGraphicsResources();
	
	//Layout y Dibujo
	void    CalculateLayout(RECT rc);
	void	DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth);
	void	DrawEsfera();
	void	RenderScene();
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);
};



