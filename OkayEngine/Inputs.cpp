#include "Inputs.h"

Inputs::Inputs(LPDIRECTINPUT8 dInput)
{
	dInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	dInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

	if (!DIKeyboard || !DIMouse)
		return;

	DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	DIKeyboard->SetCooperativeLevel(GetHWindow(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	DIMouse->SetDataFormat(&c_dfDIMouse);
	DIMouse->SetCooperativeLevel(GetHWindow(), DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

}
