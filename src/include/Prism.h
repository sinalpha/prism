#pragma once

#include <stdio.h>
#include <iostream>
#include <memory> 

#include <Windows.h>

#include "WindowController.h"
#include "ConsoleController.h"
#include "D3DController.h"
#include "SceneManager.h"
#include "ShaderReader.h"
#include "helper.h"
#include "Model.h"


class Prism {
public:

	Prism() = default;
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism() = default;

	bool Initialize(HINSTANCE);
	int Run();

	WindowController& GetWindowController() { return mWindowController; };
	ConsoleController& GetConsoleController() { return mConsoleController; };
	D3DController& GetD3DController() { return mD3DController; };
	SceneManager& GetSceneManager() { return mSceneManager; };

private:

	WindowController mWindowController;
	ConsoleController mConsoleController;
	D3DController mD3DController;
	SceneManager mSceneManager;


};
