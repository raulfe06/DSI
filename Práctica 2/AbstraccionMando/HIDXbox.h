#pragma once
#include "BaseHID.h"
#include <Windows.h>
#include <XInput.h>
#include "HID.h"
class HIDXbox : public BaseHID
{
	struct CONTROLER_STATE
	{
		XINPUT_STATE state;
		XINPUT_STATE Offstate;

		DWORD dwResult;
		XINPUT_VIBRATION vibration;
	};

private:
	CONTROLER_STATE Xbox;
public:
	HIDXbox(float t) :BaseHID(t) {};
	bool LeeMando();
	void EscribeMando();
	void Mando2HID();
};

