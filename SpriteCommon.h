#pragma once
#include "DirectXCommon.h"

class SpriteCommon
{
public:

	void Initialize(DirectXCommon* dxcommon);

	void RootsignatureCreate();
	void PipelineStateCreate();
	void CommandListCreate();

	DirectXCommon* GetDXCommon() { return dxCommon_; }

private:

	SpriteCommon* spriteCommon = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};