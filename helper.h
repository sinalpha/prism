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

