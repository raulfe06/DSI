#include "HIDXbox.h"



HIDXbox::~HIDXbox()
{
}

bool HIDXbox::LeeMando()
{
	Xbox.dwResult = XInputGetState(0, &Xbox.state);
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

	Xbox.state.Gamepad.sThumbLX -= Xbox.Offstate.Gamepad.sThumbLX;
	Xbox.state.Gamepad.sThumbLY -= Xbox.Offstate.Gamepad.sThumbLY;
	Xbox.state.Gamepad.sThumbRX -= Xbox.Offstate.Gamepad.sThumbRX;
	Xbox.state.Gamepad.sThumbRY -= Xbox.Offstate.Gamepad.sThumbRY;

	if (Xbox.state.Gamepad.sThumbLX > INPUT_DEADZONE) Xbox.state.Gamepad.sThumbLX -= INPUT_DEADZONE;
	else if (Xbox.state.Gamepad.sThumbLX < -INPUT_DEADZONE) Xbox.state.Gamepad.sThumbLX += INPUT_DEADZONE;
	else Xbox.state.Gamepad.sThumbLX = 0;

	if (Xbox.state.Gamepad.sThumbLY > INPUT_DEADZONE) Xbox.state.Gamepad.sThumbLY -= INPUT_DEADZONE;
	else if (Xbox.state.Gamepad.sThumbLY < -INPUT_DEADZONE) Xbox.state.Gamepad.sThumbLY += INPUT_DEADZONE;
	else Xbox.state.Gamepad.sThumbLY = 0;

	if (Xbox.state.Gamepad.sThumbRX > INPUT_DEADZONE) Xbox.state.Gamepad.sThumbRX -= INPUT_DEADZONE;
	else if (Xbox.state.Gamepad.sThumbRX < -INPUT_DEADZONE) Xbox.state.Gamepad.sThumbRX += INPUT_DEADZONE;
	else Xbox.state.Gamepad.sThumbRX = 0;

	if (Xbox.state.Gamepad.sThumbRY > INPUT_DEADZONE) Xbox.state.Gamepad.sThumbRY -= INPUT_DEADZONE;
	else if (Xbox.state.Gamepad.sThumbRY < -INPUT_DEADZONE) Xbox.state.Gamepad.sThumbRY += INPUT_DEADZONE;
	else Xbox.state.Gamepad.sThumbRY = 0;

	fThumbLX = (float)Xbox.state.Gamepad.sThumbLX / (float)(MAXINT16 - INPUT_DEADZONE);
	fThumbLY= (float)Xbox.state.Gamepad.sThumbLY / (float)(MAXINT16 - INPUT_DEADZONE);
	fThumbRY = (float)Xbox.state.Gamepad.sThumbRY / (float)(MAXINT16 - INPUT_DEADZONE);
	fThumbRX = (float)Xbox.state.Gamepad.sThumbRX / (float)(MAXINT16 - INPUT_DEADZONE);
}
