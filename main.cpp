#include <Windows.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <numbers>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <corecrt_math_defines.h>
#include "Input.h"
#include "DirectXCommon.h"
#include "StringUtility.h"
#include <format>
#include "D3ResouceLeakChecker.h"
#include "MyMatrix.h"

using namespace StringUtility;
#pragma comment(lib,"dxcompiler.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;

	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	//VertexData
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	//ファイルから読んだ1行を格納する
	std::string line;
	//ファイルを読み取る
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	//構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の義別子 (v ,vt, vn, f) を読み取る	

		//modeldataの建築
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z; //左から順に消費 = 飛ばしたりはできない
			position.s = 1.0f;

			//反転
			position.x *= 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;

			//反転
			normal.x *= 1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];

			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //  "/"でインデックスを区切る
					elementIndices[element] = std::stoi(index);

				}


				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);

				triangle[faceVertex] = { position,texcoord,normal };

			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// mateialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//CoInitializeEx(0, COINIT_MULTITHREADED);

	D3ResouceLeakChecker LeakChecker;

#pragma region Windouの生成
	WinApp* winApp = nullptr;

	winApp = new WinApp();
	winApp->Initialize();

	Input* input = nullptr;
	        input = new Input();
	        input->Initialize(winApp);

#pragma endregion

	DirectXCommon* dxCommon = nullptr;

	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);


	ShowWindow(winApp->GetHwnd(), SW_SHOW);


	//textureを読んで転送
	DirectX::ScratchImage mipImages3 = dxCommon->LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages3.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResources2 = dxCommon->UploadTextureData(textureResource2, mipImages3);

	//ID3D12Resource* depthStencilResource2 = CreateDepthStencilTextureResource(device, WinApp::kClientWidth,  WinApp::kClientHeight);

	//DSVようのヒープでディスクリプタの数1、shader内で触らないのでfalse
	//ID3D12DescriptorHeap* dsvDescriptorHeap2 = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//DSV生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dscDesc2{};
	dscDesc2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dscDesc2.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVHeapの先頭
	//device->CreateDepthStencilView(depthStencilResource2, &dscDesc2, dsvDescriptorHeap2->GetCPUDescriptorHandleForHeapStart());


	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeap場所決め
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(),dxCommon->GetDescriptorSizeSRV(),2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(),dxCommon->GetDescriptorSizeSRV(),2);
	//先頭ImGui
	/*textureSrvHandleCPU2.ptr += descriptorSizeSRV;
	textureSrvHandleGPU2.ptr += descriptorSizeSRV;*/
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//モデルの読み込み
	ModelData modelData = LoadObjFile("resources", "plane.obj");

	//textureを読んで転送
	//DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	/*const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CrateTextureResource(device, metadata);
	UploadTextureData(textureResource, mipImages);

	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device,  WinApp::kClientWidth,  WinApp::kClientHeight);*/
	DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResources = dxCommon->UploadTextureData(textureResource, mipImages2);
	
	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeap場所決め
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//先頭ImGui
	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

#pragma region PSO


	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成__
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;//Object3d.PS.hlsl の b0

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;//Object3d.VS.hlsl の b0

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);



	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBV
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//plxelshader
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号

	//2でまとめる

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//shaderのコンパイラ
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


	//DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//PSOここ絶対最後
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

#pragma endregion


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);


	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;

	vertexBufferView.StrideInBytes = sizeof(VertexData);


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource( sizeof(Matrix4x4));

	Matrix4x4* wvpDate = nullptr;

	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate));

	*wvpDate = MakeIdentity4x4();

	VertexData* vertexData = nullptr;

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//leftTop
	vertexData[0].position = { -0.5f, -0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f, 0.0f, -1.0f };

	//Top
	vertexData[1].position = { 0.0f, 0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
	//rightBottom
	vertexData[2].position = { 0.5f, -0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f, 0.0f, -1.0f };


	////leftTop
	//vertexData[3].position = { -0.5f, -0.5f,0.5f,1.0f };
	//vertexData[3].texcoord = { 0.0f,1.0f };
	//vertexData[3].normal = { 0.0f, 0.0f, -1.0f };

	////Top
	//vertexData[4].position = { 0.0f, 0.0f,0.0f,1.0f };
	//vertexData[4].texcoord = { 0.5f,0.0f };
	//vertexData[4].normal = { 0.0f, 0.0f, -1.0f };

	////rightBottom
	//vertexData[5].position = { 0.5f, -0.5f,-0.5f,1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };
	//vertexData[5].normal = { 0.0f, 0.0f, -1.0f };


	uint32_t SphereVertexNum = 16 * 16 * 6;

	//Sphere
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere =dxCommon->CreateBufferResource( sizeof(VertexData) * SphereVertexNum);


	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};

	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();

	vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * SphereVertexNum;

	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSphere = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	TransformationMatrix* wvpDateSphere = nullptr;

	wvpResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&wvpDateSphere));

	wvpDateSphere->World = MakeIdentity4x4();

	VertexData* vertexDataSphere = nullptr;

	vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));





	//Sprite
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 4);

	//頂点バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭アドレス
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//頂点サイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);


	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite =dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	TransformationMatrix* transformationMatrixDataSprite = nullptr;

	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	transformationMatrixDataSprite->World = MakeIdentity4x4();
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();



	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//0
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	//vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//1,3
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	//vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//2,5
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//4
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	//vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };


	//Sprite
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	//頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//リソースの先頭アドレス
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 3;
	//頂点サイズ
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;


	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceModel =dxCommon->CreateBufferResource( sizeof(VertexData) * modelData.vertices.size());

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewModel{};
	vertexBufferViewModel.BufferLocation = vertexResourceModel->GetGPUVirtualAddress();
	vertexBufferViewModel.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferViewModel.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataModel = nullptr;
	vertexResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataModel));
	std::memcpy(vertexDataModel, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());



	////三角用マテリアル
	//マテリアル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource =dxCommon->CreateBufferResource( sizeof(Vector4));
	//マテリアルにデータを書き込む
	Vector4* materialDate = nullptr;
	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//色の設定
	*materialDate = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	


	//球体用マテリアル
	//マテリアル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere =dxCommon->CreateBufferResource( sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialDateSphere = nullptr;
	//書き込むためのアドレス
	materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialDateSphere));
	//色の設定
	materialDateSphere->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDateSphere->enableLighting = true;
	materialDateSphere->uvTransform = MakeIdentity4x4();



	//球体マテリアルのライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource =dxCommon->CreateBufferResource( sizeof(DirectionalLight));
	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightSphereData = nullptr;
	//書き込むためのアドレス
	directionalLightSphereResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightSphereData));
	//色の設定
	directionalLightSphereData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightSphereData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightSphereData->intensity = 1.0f;

	//spriteのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite =dxCommon->CreateBufferResource( sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialDateSprite = nullptr;
	//書き込むためのアドレス
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDateSprite));
	//色の設定
	materialDateSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDateSprite->enableLighting = false;
	materialDateSprite->uvTransform = MakeIdentity4x4();

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,-10.5f} };

	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	Transform transformSphere{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	Transform transformL{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };


	Transform uvTransformSprite{
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f }
	};


	float *inputMaterial[3] = { &materialDate->x,&materialDate->y,&materialDate->z };
	float* inputTransform[3] = { &transform.translate.x,&transform.translate.y,&transform.translate.z };
	float* inputRotate[3] = { &transform.rotate.x,&transform.rotate.y,&transform.rotate.z };
	float* inputScale[3] = { &transform.scale.x,&transform.scale.y,&transform.scale.z };


	float* inputMaterialSphere[3] = { &materialDateSphere->color.x,&materialDateSphere->color.y,&materialDateSphere->color.z };
	float* inputTransformSphere[3] = { &transformSphere.translate.x,&transformSphere.translate.y,&transformSphere.translate.z };
	float* inputRotateSphere[3] = { &transformSphere.rotate.x,&transformSphere.rotate.y,&transformSphere.rotate.z };
	float* inputScaleSphere[3] = { &transformSphere.scale.x,&transformSphere.scale.y,&transformSphere.scale.z };
	float textureChange = 0;


	float* inputMaterialLigth[3] = { &directionalLightSphereData->color.x,&directionalLightSphereData->color.y,&directionalLightSphereData->color.z };
	float* inputDirectionLight[3] = { &directionalLightSphereData->direction.x,&directionalLightSphereData->direction.y,&directionalLightSphereData->direction.z };
	float* intensity = &directionalLightSphereData->intensity;

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (true) {

		if (winApp->ProcessMessage()) {
			break;
		}
		else {
			input->Update();

			  // 描画前処理
            dxCommon->PreDraw();

			

			if (input->Triggerkey(DIK_SPACE)) {
				OutputDebugStringA("Hit 0\n");
			}
	     
			//ゲームの処理

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			transform.rotate.y += 0.03f;

			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
			//Matrix4x4 projectionMatrix = MakeOrthographicMatrix((float)scissorRect.left, (float)scissorRect.top, (float)scissorRect.right, (float)scissorRect.bottom, 0.1f, 100.0f);
			Matrix4x4 WorldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

			//三角
			*wvpDate = WorldViewProjectionMatrix;


			//球体

			Matrix4x4 worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
			Matrix4x4 WorldViewProjectionMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));

			wvpDateSphere->WVP = WorldViewProjectionMatrixSphere;


			directionalLightSphereData->direction = Normalize(directionalLightSphereData->direction);


			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//Matrix4x4 cameraMatrixSprite = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			//Matrix4x4 projectionMatrixSprite = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, (float) WinApp::kClientWidth, (float) WinApp::kClientHeight, 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;


			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDateSprite->uvTransform = uvTransformMatrix;

			//開発用UIの処理
			ImGui::ShowDemoWindow();

			//ここにテキストを入れられる
			ImGui::Text("ImGuiText");

			ImGui::Text("Sphere");
			ImGui::InputFloat3("MaterialSphere", *inputMaterialSphere);
			ImGui::SliderFloat3("SliderMaterialSphere", *inputMaterialSphere, 0.0f, 1.0f);

			ImGui::InputFloat3("VertexSphere", *inputTransformSphere);
			ImGui::SliderFloat3("SliderVertexSphere", *inputTransformSphere, -5.0f, 5.0f);

			ImGui::InputFloat3("RotateSphere", *inputRotateSphere);
			ImGui::SliderFloat3("SliderRotateSphere", *inputRotateSphere, -10.0f, 10.0f);

			ImGui::InputFloat3("ScaleSphere", *inputScaleSphere);
			ImGui::SliderFloat3("SliderScaleSphere", *inputScaleSphere, 0.5f, 5.0f);

			ImGui::InputFloat("SphereTexture", &textureChange);

			ImGui::Text("Sprite");
			ImGui::InputFloat("SpriteX", &transformSprite.translate.x);
			ImGui::SliderFloat("SliderSpriteX", &transformSprite.translate.x, 0.0f, 1000.0f);

			ImGui::InputFloat("SpriteY", &transformSprite.translate.y);
			ImGui::SliderFloat("SliderSpriteY", &transformSprite.translate.y, 0.0f, 600.0f);

			ImGui::InputFloat("SpriteZ", &transformSprite.translate.z);
			ImGui::SliderFloat("SliderSpriteZ", &transformSprite.translate.z, 0.0f, 0.0f);


			ImGui::DragFloat2("UVTranlate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);



			


			

			dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
			dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//三角
			dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);	
			//dxCommon->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);

			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSphere->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]

			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceSphere->GetGPUVirtualAddress());
			//Yosida:ここでテクスチャの変更を行う

			if (textureChange == 0) {
				dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			}
			else {
				dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU2);
			}

			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightSphereResource->GetGPUVirtualAddress());

			//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


			//UI
			//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			//dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);

			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]

			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());

			//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			//dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//実際のcommandListのImGui描画コマンドを挟む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());


			// 描画後処理
            dxCommon->PostDraw();
			

			////// 出力ウィンドウへの文字出力
			////OutputDebugStringA("Hello DirectX!\n");
			

		}
	}

	winApp->Finalize();

	//入力解放
	delete winApp;
	dxCommon->Finalize();
	delete dxCommon;
	delete input;


	return 0;
}