#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") 

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include "ShaderReader.h"
#include "helper.h"
#include "Model.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;


class Prism {
public:


	Prism(HINSTANCE pHInstance);
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism();


	bool initialize();
	static LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	int run();


private:

	void getHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);

	bool initWindow();
	bool inItConsole();
	bool initDx3D();
	bool initDeviceAndCommandInterfaces(ComPtr<IDXGIFactory4>& factory);
	bool initFence();
	bool initBackBuffer(ComPtr<IDXGIFactory4>& factory);
	bool initConstantBufferViewHeap();

	void createPipeLine();

	void clearBackBuffer();
	void presentBackBuffer();
	void waitDrawDone();

	static const UINT frameCount{ 2 };
	static const bool useWarpDevice{ false };

	// variable for setting window;
	HINSTANCE hInstance{ NULL };
	WNDCLASSEX windowClass{ NULL };
	HWND hWindow{ NULL };
	LPCWSTR	windowTitle{ L"prism" };
	int clientWidth{ 1280 };
	int clientHeight{ 720 };
	int displayWidth{ GetSystemMetrics(SM_CXSCREEN) };
	int displayHeight{ GetSystemMetrics(SM_CYSCREEN) };
	int clientLeft{ (displayWidth - clientWidth) / 2 };
	int clientTop{ (displayHeight - clientHeight) / 2 };
	float aspect{ static_cast<float> (clientWidth / static_cast<float>(clientHeight)) };

	// Pipeline objects.
	//device
	ComPtr<ID3D12Device> device;
	
	//command
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;

	// Synchronization objects.
	//fence
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent{ NULL };
	UINT64 fenceValue{ 0 };

	//debug
	HRESULT Hr{ 0 };

	//resources
	//back buffer
	ComPtr<ID3D12Resource> renderTargets[frameCount];
	ComPtr<IDXGISwapChain4> swapChain;
	UINT frameIndex{ 0 };
	ComPtr<ID3D12DescriptorHeap> bbvHeap;
	UINT bbvDescriptorSize{ 0 };

	ComPtr<ID3D12DescriptorHeap> constantBufferViewHeap{ };
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer{ };

	void CreateConstantBuffer() {

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = 256;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBuffer)
		);






	}

};
