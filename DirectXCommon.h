#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include "WinApp.h"
#include <array>
#include <cassert>
#include <dxgi1_4.h>
class DirectXCommon
{
public:
	
	void Initialize();

	void Update();

	

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

private:

	void DeviceInitialization();

	void CommandInitialization();

	void SwapChainGenerate();

	void DepthBufferGenerate();

	void DescriptorHeapGenerate();

	void RenderTargetViewInitialization();

	void DepthStencilViewInitialization();

	void FenceInitialization();

	void ViewportRectangleInitialization();

	void ScissorRectangleInitialization();

	void DXCCompilerGenerate();

	void ImGuiInitialization();
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

	
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap; 
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	int32_t width;  
    int32_t height;
	uint32_t descriptorSizeSRV;
    uint32_t descriptorSizeRTV;
    uint32_t descriptorSizeDSV;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc; // 追加
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc; // 追加


	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesciptors, bool shaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.NumDescriptors = numDesciptors;
	descHeapDesc.Type = heapType;
	descHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	HRESULT hr = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	return descriptorHeap;
     };

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	WinApp* winApp = nullptr;

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,uint32_t descriptorSize , uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
};

