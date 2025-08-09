#pragma once

#include <string>
#include <vector>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d12.h>
#include "d3dx12.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Texture.h"
#include "Material.h"
#include "helper.h"

class BasicMesh {
public:

	BasicMesh() = delete;
	~BasicMesh() = default;

	BasicMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 worldViewProj = Identity4x4();
	};

	bool initFromScene(const aiScene* scene, const std::string& fileName);
	void countVerticesAndIndices(const aiScene* scene, unsigned int& numVertices, unsigned int& numIndices);
	void initAllMeshs(const aiScene* scene);
	void initSingleMesh(const aiMesh* paiMesh);
	bool initMaterials(const aiScene* scene, const std::string& fileName);
	void populateBuffer();
	void populateConstantBuffer();

	bool loadMesh(const std::string& fileName);


	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const;
	int getVertexNum() const;

	std::vector<unsigned int> indices;
	DirectX::XMFLOAT4X4 world{ Identity4x4() };
	DirectX::XMFLOAT4X4 view{ Identity4x4() };
	DirectX::XMFLOAT4X4 proj{ Identity4x4() };

	ObjectConstants* mappedConstBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> constantBufferHeap{ nullptr };

private:

	struct Vertex {
		DirectX::XMFLOAT3 position;
	};

	struct BasicMeshEntry {

		BasicMeshEntry() = default;

		unsigned int numIndices{ 0 };
		unsigned int baseVertex{ 0 };
		unsigned int baseIndex{ 0 };
		unsigned int materialIndex{ 0 };
	};

	std::vector<BasicMeshEntry> meshes;
	std::vector<Texture*> textures;
	std::vector<Vertex> vertices;
	std::vector<Material> materials;

	Microsoft::WRL::ComPtr<ID3D12Device> device{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer{ nullptr };



	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{ 0 };
};