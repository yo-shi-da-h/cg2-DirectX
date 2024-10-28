#pragma once
#include "Windows.h"
#include "wrl.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
class Input
{
public:

	void Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool PushKey(BYTE keyNumber);

	bool Triggerkey(BYTE keyNumber);
private:
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8> directInput;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
};

