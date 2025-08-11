#include "Model.h"

Model::Model(
	ID3D12Device* pDevice,
	ID3D12GraphicsCommandList* pCommandList)
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

int Model::GetVerticesNum() {
	return mNumVertices;
}

void Model::SetConstantViewToHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pConstantBufferViewHeap, Microsoft::WRL::ComPtr<ID3D12Resource>& mConstantBuffer) {

	mConstantBuffer->Map(0, nullptr, (void**)&mMappedBuffer);

	auto hConstantBufferViewHeap = pConstantBufferViewHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<UINT>(mConstantBuffer->GetDesc().Width);
	mDevice->CreateConstantBufferView(&desc, hConstantBufferViewHeap);

}

void Model::InitFromScene(const aiScene* pScene, const std::string& pFileName) {

	mMeshes.resize(pScene->mNumMeshes);

	CountVertices(pScene);

	InitAllMeshs(pScene);

	PopulateVertexBuffer();


}

void Model::CountVertices(const aiScene* pScene) {


	for (unsigned int i{ 0 }; i < mMeshes.size(); ++i) {
		mMeshes[i].baseVertex = mNumVertices;

		mNumVertices += pScene->mMeshes[i]->mNumVertices;
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




