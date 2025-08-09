#pragma once

#include <vector>
#include <string>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Vertex.h"
#include "MathHelper.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

class Model {
public:

	Model() = default;
	~Model() = default;

	Model(const Microsoft::WRL::ComPtr<ID3D12Device>&, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&);

	void LoadModel(const std::string&);
	
	DirectX::XMFLOAT4X4& GetView();
	DirectX::XMFLOAT4X4& GetProj();
	DirectX::XMFLOAT4X4& GetWorld();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	int GetVerticesNum();

private:

	struct BasicMeshEntry {
		unsigned int baseVertex{ 0 };
	};

	void InitFromScene(const aiScene* , const std::string&);
	void CountVertices(const aiScene*);
	void InitAllMeshs(const aiScene*);
	void InitSingleMesh(const aiMesh*);
	void PopulateBuffer();

	Microsoft::WRL::ComPtr<ID3D12Device> mDevice{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{ 0 };

	std::vector<BasicMeshEntry> mMeshes;
	std::vector<Vertex> mVertices;
	UINT mNumVertices{ 0 };
	
	DirectX::XMFLOAT4X4 mView{ MathHelper::Identity4x4() };
	DirectX::XMFLOAT4X4 mProj{ MathHelper::Identity4x4()};
	DirectX::XMFLOAT4X4 mWorld{ MathHelper::Identity4x4()};

};



