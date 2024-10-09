#include "Input.h"
#include <cassert>


void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;
	
	result = DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	
	result = directInput->CreateDevice(GUID_SysKeyboard,&keyboard,NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	keyboard->Acquire();

	keyboard->GetDeviceState(sizeof(key),key);

	

	memcpy(keyPre,key, sizeof(key));
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber]) {
		OutputDebugStringA("Hit 0\n");
		return true;
	}
	return false;
}

bool Input::Triggerkey(BYTE keyNumber)
{
	if (keyPre[keyNumber]==0) {
		OutputDebugStringA("Hit 0\n");
		return true;
	}
	return false;
}
