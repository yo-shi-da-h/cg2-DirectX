#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include "WinApp.h"
#include <array>
#include <cassert>
#include <dxgi1_4.h>
#include <dxcapi.h>
class DirectXCommon
{
public:
	
	void Initialize(WinApp* winApp);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,uint32_t descriptorSize , uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

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

	void PreDraw();

	void PostDraw();
	
	 //dxgiFactoryの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	 //コマンドキュー生成
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
    //コマンドアロケータを生成する
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>commandAllocator = nullptr;
    //コマンドリストを生成する
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};



    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr; 
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;

	int32_t width;  
    int32_t height;
	uint32_t descriptorSizeSRV = 0;
    uint32_t descriptorSizeRTV = 0;
    uint32_t descriptorSizeDSV = 0;;
	//SwapChain
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{}; // 追加
	//swapchainからリソースを引っ張る
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	 //DXCの初期化
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;
    //include対応のため設定しておく
    IDxcIncludeHandler* includeHandler = nullptr;

    //フェンスの生成
    Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
    UINT64 fenceValue = 0;
    HANDLE fenceEvent = nullptr;

    D3D12_VIEWPORT viewport{};
    D3D12_RECT scissorRect{};
   


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
	
  

	
    //D3D12_RESOURCE_DESC depthStencilDesc = {};
    

	WinApp* winApp = nullptr;

	
};

