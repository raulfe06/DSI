#include "HIDXbox.h"



HIDXbox::~HIDXbox()
{
}

bool HIDXbox::LeeMando()
{
	Xbox.dwResult = GetInputState();
	if(Xbox.dwResult == ERROR_SUCCESS) return true;
	else return false;

}

void HIDXbox::EscribeMando()
{
	Xbox.vibration.wLeftMotorSpeed = (WORD)fLeftVibration * MAXINT16;
	Xbox.vibration.wRightMotorSpeed = (WORD)fRightVibration * MAXINT16;
	XInputSetState(0, &Xbox.vibration);
}

void HIDXbox::Mando2HID()
{
	wButtons = Xbox.state.Gamepad.wButtons;
	fLeftTrigger = (float)Xbox.state.Gamepad.bLeftTrigger / (float )MAXBYTE;
	fRightTrigger = (float)Xbox.state.Gamepad.bRightTrigger / (float)MAXBYTE;

		if(xbox.state.gamepad.sthumb.x > input JOY_USEDEADZONE)
			fThumbLX = (float)xbox.state.gamepad.
}
