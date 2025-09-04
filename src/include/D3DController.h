#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") 

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "ShaderReader.h"
#include "helper.h"
#include "DescriptorHeapAllocator.h"

class Prism;
class D3DController {
public:

	void Init(Prism*);
	void Render();

	inline Microsoft::WRL::ComPtr<ID3D12Device>& GetDevicePtr() { return mDevice; }
	inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GetCommandListPtr() { return mCommandList; }
	inline DescriptorHeapAllocator& GetDescriptorHeapAllocator() { return mDescriptorHeapAllocator; }

private:

	Prism* mPrism{ nullptr };
	DescriptorHeapAllocator mDescriptorHeapAllocator;

	void GetHardwareAdapter(IDXGIFactory2*, IDXGIAdapter1**);

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

	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
	CD3DX12_VIEWPORT mViewport;
	CD3DX12_RECT mScissorRect;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent{ NULL };
	UINT64 mFenceValue{ 0 };

	HRESULT mHr{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargets[scmFrameCount];
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	UINT mFrameIndex{ 0 };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mBackBufferViewHeap;
	UINT mBackBufferViewDescriptorSize{ 0 };

};