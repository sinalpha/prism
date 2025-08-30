#pragma once

#include "WindowController.h"
#include "ConsoleController.h"
#include "D3DController.h"
#include "Scene.h"

class PrismBase {
public:

	virtual ~PrismBase() = 0;
	virtual WindowController& GetWindowController();
	virtual ConsoleController& GetConsoleController();
	virtual D3DController& GetD3DController();
	virtual Scene* GetCurrentScene() ;
};