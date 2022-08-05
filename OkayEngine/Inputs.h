#pragma once

#include "Okay.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class Inputs
{
public:
	Inputs(LPDIRECTINPUT8 dInput);

	void ReadEvents();

public: // Keyboard
	bool KeyDown(int key) const;
	bool KeyReleased(int key) const;


public: // Mouse
	void Lock(bool lock);

	bool IsLocked() const;

	bool LeftDown() const;
	bool RightDown() const;

	bool LeftReleased() const;
	bool RightReleased() const;

	UINT GetXPos() const;
	UINT GetYPos() const;
	
	int GetDeltaX() const;
	int GetDeltaY() const;


private: // Keyboard
	bool keysDown[256];
	bool keysReleased[256];

	IDirectInputDevice8* DIKeyboard;


private: // Mouse
	UINT xPos, yPos;
	bool leftReleased, rightReleased;

	bool locked;

	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE lastState;
};