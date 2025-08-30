#pragma once
#include "Model.h"

#include "PrismBase.h"

class Scene {
public:

	void Init(PrismBase*);
	void Update();

	inline Model& GetModel() { return mModel; };

private:

	PrismBase* mPrism;

	Model mModel;

};