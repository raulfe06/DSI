#include "BaseHID.h"


BaseHID::~BaseHID()
{
}

void BaseHID::Actualiza()
{
	wLastButtons = wButtons; //Copia estado de botones
	bConected = LeeMando(); //Leo Mando
	if (bConected == true)
	{
		Mando2HID(); //Vuelco de Mando a HID normalizando
		wButtonsDown = (-wLastButtons) & wButtons;
		wButtonsUp = (wLastButtons)& wButtons;
					 //Actualizo Gestos de entrada genéricos (entradas)
					 //Genero Gesto de feedback (salida)
		fThumbLXf = (1 - a)*fThumbLXf* +a * fThumbLX;
		fThumbLYf = (1 - a)*fThumbLYf* +a * fThumbLY;

		fThumbRXf = (1 - a)*fThumbRXf* +a * fThumbRX;
		fThumbRYf = (1 - a)*fThumbRYf* +a * fThumbRY;



		if ((fThumbLX > 0) & (fThumbLY > 0)) {
			
			}
		EscribeMando(); //Escribo en Mando el feedback
	}
}
