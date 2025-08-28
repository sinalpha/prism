#pragma once

#include <DirectXMath.h>

#include "Manager.h"
#include "WindowManager.h"
#include "ModelManager.h"

class SceneManager : public Manager {
public:

	void Init(Prism*);
	void Update(WindowManager&);

private:

	ModelManager mModelManager;

};