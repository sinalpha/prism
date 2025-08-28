#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Model.h"

using ModelPtr = std::unique_ptr<Model>;

class ModelManager {
public:

	ModelManager(Microsoft::WRL::ComPtr<ID3D12Device>&, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&);

	ModelPtr& operator[](const int);

	void Load(std::string&);

private:
	int mModelCount{ 0 };
	int mCapacity{ 0 };
	std::vector<ModelPtr> mModels;

	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
};