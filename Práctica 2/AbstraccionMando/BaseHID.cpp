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
		fthumlxf = (1-a)*fthumslxf + a*fThumbLX

			if(fThumbLX>0) & fThumbLY
		EscribeMando(); //Escribo en Mando el feedback
	}
}
