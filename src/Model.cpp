#include "Model.h"

Model::Model(
	Microsoft::WRL::ComPtr<ID3D12Device>&  pDevice,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	mDevice = pDevice;
	mCommandList = pCommandList;
}

void Model::LoadModel(const std::string& pFileName) {

	Assimp::Importer importer;

	const aiScene* scene{ importer.ReadFile(pFileName, ASSIMP_LOAD_FLAGS) };

	if (scene) {
		InitFromScene(scene, pFileName);
	}
	else {
		MessageBox(0, L"Error parsing  Failed.", 0, 0);
	}

}

void Model::SetMat(DirectX::XMMATRIX& pMat) {


	mMappedBuffer->mat = pMat;
}

DirectX::XMFLOAT4X4& Model::GetView(){
	return mView; 
}

DirectX::XMFLOAT4X4& Model::GetProj(){
	return mProj;
};

DirectX::XMFLOAT4X4& Model::GetWorld() {
	return mWorld; 
};

D3D12_VERTEX_BUFFER_VIEW Model::GetVertexBufferView() {
	return mVertexBufferView;
};

D3D12_INDEX_BUFFER_VIEW Model::GetIndexBufferView() {
	return mIndexBufferView;
}

int Model::GetVerticesNum() {
	return mNumVertices;
}

int Model::GetIndicesNum() {
	return mNumIndices;
}

ID3D12DescriptorHeap** Model::GetDescriptorHeapAddress() {
	return mDescriptorHeap.GetAddressOf();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Model::GetDescriptorHeap() {
	return mDescriptorHeap;
}

void Model::InitFromScene(const aiScene* pScene, const std::string& pFileName) {

	mMeshes.resize(pScene->mNumMeshes);

	CountVerticesAndIndices(pScene);

	InitAllMeshs(pScene);

	PopulateVertexBuffer();
	PopulateIndexBuffer();
	CreateDescriptorHeap();
	CreateConstantBuffer();
	SetConstantViewToHeap();
}

void Model::CountVerticesAndIndices(const aiScene* pScene) {


	for (unsigned int i{ 0 }; i < mMeshes.size(); ++i) {
		mMeshes[i].baseVertex = mNumVertices;
		mMeshes[i].numIndices = pScene->mMeshes[i]->mNumFaces * 3;
		mMeshes[i].baseVertex = mNumVertices;
		mMeshes[i].baseIndex = mNumIndices;

		mNumVertices += pScene->mMeshes[i]->mNumVertices;
		mNumIndices += mMeshes[i].numIndices;
	}
}

void Model::InitAllMeshs(const aiScene* pScene) {

	for (unsigned int i{ 0 }; i < mMeshes.size(); ++i) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(paiMesh);
	}


}

void Model::InitSingleMesh(const aiMesh* pAiMesh) {

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	Vertex v;

	// Populate the vertex attribute vectors
	for (unsigned int i{ 0 }; i < pAiMesh->mNumVertices; i++) {

		const aiVector3D& pPos{ pAiMesh->mVertices[i] };
		v.position = DirectX::XMFLOAT3(pPos.x, pPos.y, pPos.z);

		mVertices.push_back(v);
	}

	for (unsigned int i{ 0 }; i < pAiMesh->mNumFaces; i++) {
		const aiFace& face = pAiMesh->mFaces[i];
		if (face.mNumIndices != 3) {
			MessageBox(0, L"Only support triangle mesh.", 0, 0);
			return;
		}
		mIndices.push_back(face.mIndices[0]);
		mIndices.push_back(face.mIndices[1]);
		mIndices.push_back(face.mIndices[2]);
	}

}

void Model::PopulateVertexBuffer() {

	const UINT vertexBufferByteSize = sizeof(Vertex) * mNumVertices;

	D3D12_HEAP_PROPERTIES prop;
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = vertexBufferByteSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	mDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mVertexBuffer.GetAddressOf()));

	UINT8* mappedPositionBuf;
	mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedPositionBuf));
	memcpy(mappedPositionBuf, mVertices.data(), vertexBufferByteSize);
	mVertexBuffer->Unmap(0, nullptr);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = vertexBufferByteSize;
	mVertexBufferView.StrideInBytes = sizeof(Vertex);

}

void Model::PopulateIndexBuffer() {

	const UINT indexBufferByteSize = sizeof(UINT) * mNumIndices;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = indexBufferByteSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	mDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mIndexBuffer.GetAddressOf()));

	UINT8* mappedIndexBuf = nullptr;
	mIndexBuffer->Map(0, nullptr, (void**)&mappedIndexBuf);
	memcpy(mappedIndexBuf, mIndices.data(), indexBufferByteSize);
	mIndexBuffer->Unmap(0, nullptr);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.SizeInBytes = indexBufferByteSize;
	mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

}

void Model::CreateDescriptorHeap() {
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.NodeMask = 0;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));
	
	mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

}

void Model::CreateConstantBuffer() {
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

	mDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mConstantBuffer.GetAddressOf())
	);

}

void Model::SetConstantViewToHeap() {

	mConstantBuffer->Map(0, nullptr, (void**)&mMappedBuffer);

	auto hConstantBufferViewHeap = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<UINT>(mConstantBuffer->GetDesc().Width);
	mDevice->CreateConstantBufferView(&desc, hConstantBufferViewHeap);

}