#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>

class D3DController {
public:

	void Init();
	void Render();

private:

	void GetHardwareAdapter(IDXGIFactory2*, IDXGIAdapter1**);

};