#include "Scene.h"


Scene::Scene():pBrush(NULL), pRenderTarget(NULL), pFactory(NULL), reloj(D2D1::Ellipse(D2D1::Point2F(), 0, 0)),
ptMouse(D2D1::Point2F())

{
	
}


Scene::~Scene()
{
}


HRESULT Scene::CreaFactoria()
{
	return	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
}

void Scene::LiberaFactoria()
{
	SafeRelease(&pFactory);
}

void Scene::DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth)
{
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(fAngle, reloj.point));
	// endPoint defines one end of the hand.
	D2D_POINT_2F endPoint = D2D1::Point2F(reloj.point.x,reloj.point.y - (reloj.radiusY * fHandLength));
	// Draw a line from the center of the reloj to endPoint.
	pRenderTarget->DrawLine(reloj.point, endPoint, pStroke, fStrokeWidth);
}

void Scene::DrawReloj()
{
	reloj = D2D1::Ellipse(D2D1::Point2F(x, y), radio, radio);
	pRenderTarget->FillEllipse(reloj, pBrush);
	pRenderTarget->DrawEllipse(reloj, pStroke);
}
void Scene::DrawEsfera()
{
	pRenderTarget->FillEllipse(ellipse, pEllipse);
	pRenderTarget->DrawEllipse(ellipse, pStroke);
}
void Scene::RenderScene()
{
	pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		DrawEsfera();
		DrawReloj();
		DrawClockHand(0.6f, fHourAngle, 5);
		DrawClockHand(0.75f, fMinuteAngle, 3);
		DrawClockHand(0.95f, fSecondAngle, 2);
	HRESULT hr = pRenderTarget->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		DiscardGraphicsResources();
	}
}
// Recalculate drawing layout when the size of the window changes.

void Scene::CalculateLayout(RECT rc)
{
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	if (pRenderTarget != NULL)
	{
		pRenderTarget->Resize(size);
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		x = size.width / 8;
		y = size.height / 8;
		radio = min(x, y);
	}
}

HRESULT Scene::CreateGraphicsResources(HWND m_hwnd,RECT rc)
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
			const D2D1_COLOR_F color1 = D2D1::ColorF(0.0f, 0.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color1, &pStroke);
			const D2D1_COLOR_F color2 = D2D1::ColorF(1.0f, 0.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color2, &pEllipse);
			if (SUCCEEDED(hr))
			{
				CalculateLayout(rc);
			}
		}
	}
	return hr;
}

void Scene::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
	SafeRelease(&pStroke);
}


