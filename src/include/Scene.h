#pragma once
#include "Model.h"

class Prism;
class Scene {
public:

	void Init(Prism*);
	void Update();

	inline Model& GetModel() { return mModel; };

private:

	Prism* mPrism{ nullptr };

	Model mModel;

};