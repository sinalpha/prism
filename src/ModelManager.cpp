#include "ModelManager.h"

ModelManager::ModelManager(Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	mDevice = pDevice;
	mCommandList = pCommandList;

}

ModelPtr& ModelManager::operator[](const int index) {
	if (index < 0 || index >= mModelCount) {
		return;
	}

	return mModels[index];
}

void ModelManager::Load(std::string& pFileName) {


	ModelPtr model = std::make_unique<Model>(mDevice, mCommandList);
	model->LoadModel(pFileName);
	mModels.push_back(model);


}
