#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include "WinApp.h"
class DirectXCommon
{
public:
	
	void Initialize();

	void Update();

	void DeviceInitialization();

	void CommandInitialization();

	void SwapChainGenerate();

	void DepthBufferGenerate();

	void DescriptorHeapGenerate();

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT num)

	WinApp* winApp = nullptr;
};

