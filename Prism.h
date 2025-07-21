#pragma once

#include <Windows.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") 

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include "helper.h"

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
	bool initDx3D();

	static const UINT frameCount{ 2 };
	static const bool useWarpDevice{ false };

	// variable for setting window;
	HINSTANCE hInstance{ NULL };
	WNDCLASSEX windowClass{ NULL };
	HWND hWindow{ NULL };
	LPCWSTR	windowTitle{ L"Prism" };
	int clientWidth{ 1280 };
	int clientHeight{ 720 };
	int displayWidth{ GetSystemMetrics(SM_CXSCREEN) };
	int displayHeight{ GetSystemMetrics(SM_CYSCREEN) };
	int clientLeft{ (displayWidth - clientWidth) / 2 };
	int clientTop{ (displayHeight - clientHeight) / 2 };

	// Pipeline objects.
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> renderTargets[frameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize{ 0 };

	// App resources.
	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{ 0 };

	// Synchronization objects.
	UINT frameIndex{ 0 };
	HANDLE fenceEvent{ NULL };
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue{ 0 };
};