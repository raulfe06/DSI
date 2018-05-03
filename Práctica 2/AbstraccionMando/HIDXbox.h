#pragma once

#include "BaseHID.h"
#include <XInput.h>
#pragma comment(lib, "xinput9_1_0.lib")

#define INPUT_DEADZONE (0.24f * FLOAT(0x7FFF))


struct CONTROLER_STATE
{
	XINPUT_STATE State;
	XINPUT_STATE OffState;

	DWORD dwResult;
	XINPUT_VIBRATION vibration;
};

class HIDXBox :public BaseHID
{
public:

	HIDXBox();
	HIDXBox(float t);
	~HIDXBox();
	WORD getLast() { return wLastButtons; };
	WORD getButtons() { return wButtons; };
	virtual bool LeeMando();
	virtual void EscribeMando();
	virtual void Mando2HID();
	virtual void Calibra();
	
private:
	CONTROLER_STATE XBox;
};