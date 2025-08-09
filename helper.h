#pragma once

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

#include <exception>
#include <format>

#include <Windows.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

inline Microsoft::WRL::ComPtr<ID3D12Resource> createDefaultBuffer( ID3D12Device* device, const void* initData, UINT64 byteSize)
{

    Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;
    
    D3D12_HEAP_PROPERTIES prop;
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = sizeof(byte);
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ThrowIfFailed(device->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    UINT8* mappedPositionBuf;
    //코멘트
    //
	// reinterpret_cast에 대해서 알아오기
    ThrowIfFailed( defaultBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedPositionBuf)));
    memcpy(mappedPositionBuf, initData, sizeof(byteSize));
	defaultBuffer->Unmap(0, nullptr);

    

    return defaultBuffer.Get();
}

static DirectX::XMFLOAT4X4 Identity4x4()
{
    static DirectX::XMFLOAT4X4 I(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    return I;
}