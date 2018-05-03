#include "BaseHID.h"

BaseHID::BaseHID()
{
}

BaseHID::~BaseHID()
{
}

BaseHID::BaseHID(float t)
{
	T = t / 1000; 
	a = T / (0.1 + T); 
}

bool BaseHID::gBU(WORD bit) { return false; }

float BaseHID::gLT() { return fLeftTrigger; }

float BaseHID::gRT() { return fRightTrigger; }

float BaseHID::gLJX() { return fThumbLX; }

float BaseHID::gLJY() { return fThumbLY; }

float BaseHID::gRJX() { return fThumbRX; }

float BaseHID::gRJY() { return fThumbRY; }

float BaseHID::gLJXf() { return fThumbLXf; }

float BaseHID::gLJYf() { return fThumbLYf; }

float BaseHID::gRJXf() { return fThumbRXf; }

float BaseHID::gRJYf() { return fThumbRYf; }

void BaseHID::sLR(float cantidad, float tiempo)
{
	fLeftVibration = cantidad;
	tLR = tiempo;
}

void BaseHID::sRR(float cantidad, float tiempo)
{
	fRightVibration = cantidad;
	tRR = tiempo;
}

bool BaseHID::BD(WORD Bit)
{
	return (wButtons & Bit && !(wLastButtons & Bit));
}

bool BaseHID::BU(WORD Bit)
{
	return (!(wButtons & Bit) && (wLastButtons & Bit));
}

bool BaseHID::GRLJ()
{
	return (Ro == NEG_POS);
}

void BaseHID::Actualiza()
{
	wLastButtons = wButtons; 
	bConected = LeeMando(); 

	if (bConected)
	{
		Mando2HID(); 

		//Thumbs
		fThumbLXf = (1 - a)  *fThumbLXf + a  *fThumbLX;
		fThumbLYf = (1 - a)  *fThumbLYf + a  *fThumbLY;
		fThumbRXf = (1 - a)  *fThumbRXf + a  *fThumbRX;
		fThumbRYf = (1 - a)  *fThumbRYf + a  *fThumbRY;

		//Rotacion
		if ((fThumbLX > 0)&(fThumbLY > 0))
		{
			Ro = POS_POS;
			tRo = 1.0;
		}

		if (tRo > 0)
		{
			tRo -= T;
			if (Ro == NEG_POS)
				Ro = POS_POS;
			if ((fThumbLX > 0)&(fThumbLY <= 0)&(Ro == POS_POS))
				Ro = POS_NEG;
			if ((fThumbLX <= 0)&(fThumbLY < 0)&(Ro == POS_NEG))
				Ro = NEG_NEG;
			if ((fThumbLX < 0)&(fThumbLY >= 0)&(Ro == NEG_NEG))
				Ro = NEG_POS;
		}
		else
			Ro = POS_POS;


		if (tLR > 0)
			tLR -= T;
		else
			sLR(0.0, 0.0);

		if (tRR > 0)
			tRR -= T;
		else
			sRR(0.0, 0.0);

		EscribeMando(); 
	}
}