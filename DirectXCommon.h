#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
class DirectXCommon
{
public:
	
	void Initialize();

	void Update();

	void DeviceInitialization();

	void CommandInitialization();

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
};

