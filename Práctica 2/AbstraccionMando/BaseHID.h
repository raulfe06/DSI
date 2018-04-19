#pragma once
#include <windows.h>
#include <commdlg.h>
#include <XInput.h>
#include <basetsd.h>

class BaseHID
{
public:
	bool bConected; //Mando Conectado
					//Gets & Sets
	bool gBU(WORD bit) ;//Estado del Boton codificado en bit
	float gLT() { return fLeftTrigger; }; //Left Triger [0,1]
	float gRT(); //Right Triger [0,1]
	float gLJX(); //LeftJoyX [-1,1]
	float gLJY(); //LeftJoyY [-1,1]
	float gRJX(); //RightJoyX [-1,1]
	float gRJY(); //RightJoyY [-1,1]
	float gLJXf(); //LeftJoyXfiltered [-1,1]
	float gLJYf(); //LeftJoyYfiltered [-1,1]
	float gRJXf(); //RightJoyXfiltered [-1,1]
	float gRJYf(); //RigthJoyYfiltered [-1,1]
	void sLR(float cantidad, float tiempo){fLeftVibration = cantidad; tLR = tiempo; }//LeftRumble [0,1]: cantidad [0,1], tiempo [0,inf]
	void sRR(float cantidad, float tiempo); //RightRumble [0,1]: cantidad [0,1], tiempo [0,inf]
											//Gestos
	bool BD(WORD Bit); //Boton Down codificado en Bit
	bool BU(WORD Bit); //Boton Up codificado en Bit
	bool GRLJ(); //Gesto de Rotación del LeftJoy
	BaseHID(float t) : T(t/1000), a(T / (0.1 + T)) //Constructor que recoge el periodo de muestreo
	{
		
	};
	~BaseHID() {};
	void Actualiza(); //Actualiza Mando2HID y HID2Mando.
protected:
	//Entradas
	USHORT wButtons; //Botones (Utilizo Codificación Xbox360)
	USHORT wLastButtons; //Botones anteriores (Utilizo Codificación Xbox360)
	float fLeftTrigger, fRightTrigger; //[0.0,1.0]
	float fThumbLX, fThumbLY, fThumbRX, fThumbRY; //[-1.0,1.0]
	float fThumbLXf, fThumbLYf, fThumbRXf, fThumbRYf; //[-1.0,1.0] Filtrado
	float T; //Perido de actualización
	const float a; //Cte.Tiempo Filtro
				   //Salidas
	float fLeftVibration, fRightVibration; //[0.0,1.0] Salida
	float tLR = 0.0; //Tiempo que queda de vibración en LR
	float tRR = 0.0; //Tiempo que queda de vibración en RR
					 //Gestos
	USHORT wButtonsDown; //EventosDown Botones (Codificación Xbox360?)
	USHORT wButtonsUp; //EventosUp Botones (Codificación Xbox360?)
	//EstadosRotacion Ro; //Estado del gesto de rotación
	float tRo = 0.0; //Tiempo que queda para el gesto de rotación
					 //Funciones virtuales que se deben reimplementar para cada mando.
	virtual bool LeeMando() = 0; //Lee estado del mando
	virtual void EscribeMando() = 0; //Escribe Feeback en mando
	virtual void Mando2HID() = 0; //Vuelca el estado del mando al HID
};


