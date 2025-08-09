#include "BasicMesh.h"

BasicMesh::BasicMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
	: device{ device }, commandList{ commandList }
{

}

bool BasicMesh::loadMesh(const std::string& fileName) {

	//clear();

	bool ret{ false };
	Assimp::Importer importer;

	const aiScene* scene{ importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS) };

	if (scene) {
		ret = initFromScene(scene, fileName);
	}
	else {
		
		MessageBox(0, L"Error parsing  Failed.", 0, 0);
	}

	return ret;
}

D3D12_VERTEX_BUFFER_VIEW BasicMesh::getVertexBufferView() const {
	return vertexBufferView;
}

bool BasicMesh::initFromScene(const aiScene* scene, const std::string& fileName) {


	meshes.resize(scene->mNumMeshes);
	textures.resize(scene->mNumMaterials);

	unsigned int numVertices{ 0 };
	unsigned int numIndices{ 0 };

	countVerticesAndIndices(scene, numVertices, numIndices);

	initAllMeshs(scene);

	populateBuffer();

	return true;


}

int BasicMesh::getVertexNum() const { 
	return static_cast<int>(vertices.size()); 
}

void BasicMesh::countVerticesAndIndices(const aiScene* scene, unsigned int& numVertices, unsigned int& numIndices) {
	

	for (unsigned int i{0}; i < meshes.size(); ++i) {
		meshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		meshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].baseVertex = numVertices;
		meshes[i].baseIndex = numIndices;

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += meshes[i].numIndices;
	}
}

void BasicMesh::initAllMeshs(const aiScene* scene) {

	for (unsigned int i{ 0 }; i < meshes.size(); ++i) {
		const aiMesh* paiMesh = scene->mMeshes[i];
		initSingleMesh(paiMesh);
	}


}


void BasicMesh::initSingleMesh(const aiMesh* paiMesh) {

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	Vertex v;

	// Populate the vertex attribute vectors
	for (unsigned int i{ 0 }; i < paiMesh->mNumVertices; i++) {
		
		const aiVector3D& pPos{ paiMesh->mVertices[i] };
		v.position = DirectX::XMFLOAT3(pPos.x, pPos.y, pPos.z);

		vertices.push_back(v);
	}

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);
	}
}



bool BasicMesh::initMaterials(const aiScene* scene, const std::string& fileName) {

	std::string::size_type slashIndex = fileName.find_last_of("/");
	std::string DIR;

	if (slashIndex == std::string::npos) {
		DIR = ".";
	}
	else if (slashIndex == 0) {
		DIR = "/";
	} else {
		DIR = fileName.substr(0, slashIndex);
	}

	bool Ret = true;

	//ÄÚ¸àÆ®
	// 
	//printf("Num materials: %d\n", pScene->mNumMaterials);
	//
	//
	// Initialize the materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* pMaterial{ scene->mMaterials[i] };

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p{ path.data };

				if(p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				std::string fullPath{ DIR + "/" + p };

				//ÀÛ¾÷ ¸ØÃã
				textures[i] = new Texture();
			}
		}

	}

	return Ret;

}

void BasicMesh::populateBuffer() {

	// Create the vertex buffer and populate it with vertex data
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	vertexBuffer = createDefaultBuffer(device.Get(), vertices.data(), vbByteSize);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vbByteSize);
	vertexBufferView.StrideInBytes = sizeof(Vertex);

}

void BasicMesh::populateConstantBuffer() {

	{
		D3D12_HEAP_PROPERTIES prop;
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

	constantBuffer->Map(0, nullptr, (void**)&mappedConstBuffer);

	{
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.NodeMask = 0;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&constantBufferHeap));

		}

		auto hCbvHeap = constantBufferHeap->GetCPUDescriptorHandleForHeapStart();

		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
			desc.SizeInBytes = static_cast<UINT>(constantBuffer->GetDesc().Width);
			device->CreateConstantBufferView(&desc, hCbvHeap);
		}
	}
}