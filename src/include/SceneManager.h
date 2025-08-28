#pragma once


#include "Model.h"

class Prism;
class SceneManager {
public:

	void Init(Prism*);
	void Update();

	inline Model& GetModel() { return mModel; };

private:

	Prism* mPrism;

	Model mModel;

};