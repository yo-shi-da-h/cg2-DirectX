#include "WinApp.h"
#include <wrl.h>


LRESULT WinApp::WindowProc(HWND hwnd, UINT mag, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, mag, wparam, lparam)) {
		return true;
	}
	
}

void WinApp::Initialize()
{
	HRESULT hr = CoInitializeEx(0, COINITBASE_MULTITHREADED);

	WNDCLASS wc{};

	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"C62WindowClass";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスの登録
	RegisterClass(&wc);

	//クライアント領域のサイズ　横　縦
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	//　ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0,kClientWidth,kClientHeight };


	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	


	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::Update()
{
}
