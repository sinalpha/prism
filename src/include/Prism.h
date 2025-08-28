#pragma once

#include <stdio.h>
#include <iostream>
#include <memory> 

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") 

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include <Windows.h>

#include "WindowController.h"
#include "ShaderReader.h"
#include "helper.h"
#include "Model.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Prism {
public:


	Prism(HINSTANCE);
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism();

	bool Initialize();


	int Run();

private:

	void Update();
	void Render();

	void GetHardwareAdapter(IDXGIFactory2*, IDXGIAdapter1**);

	bool InItConsole();
	bool InitDx3D();
	bool InitScenes();
	bool InitFactory();
	bool InitDevice();
	bool InitCommandInterfaces();
	bool InitFence();
	bool InitFrameInterfaces();
	bool InitSwapChain();
	bool InitRenderTargetViewHeap();
	bool InitRenderTarget();

	void CreatePipeLine();
	void SetViewPort();
	void SetScissorRect();

	void ClearBackBuffer();
	void PresentBackBuffer();
	void WaitDrawDone();
	void SetCommandList();

	static const UINT scmFrameCount{ 2 };
	static const bool scmUseWarpDevice{ false };


	WindowController mWindowManager;

	//Direct3D 12 member variables
	ComPtr<ID3D12Device> mDevice;	
	ComPtr<IDXGIFactory4> mFactory;

	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	
	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;
	CD3DX12_VIEWPORT mViewport;
	CD3DX12_RECT mScissorRect;

	ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent{ NULL };
	UINT64 mFenceValue{ 0 };

	HRESULT mHr{ 0 };

	ComPtr<ID3D12Resource> mRenderTargets[scmFrameCount];
	ComPtr<IDXGISwapChain4> mSwapChain;
	UINT mFrameIndex{ 0 };
	ComPtr<ID3D12DescriptorHeap> mBackBufferViewHeap;
	UINT mBackBufferViewDescriptorSize{ 0 };

	//Objects to render
	Model mModel;

};
