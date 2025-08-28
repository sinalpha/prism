#pragma once

#include <Windows.h>

#include "WindowController.h"
#include "ConsoleController.h"
#include "D3DController.h"
#include "SceneManager.h"



class Prism {
public:

	Prism() = default;
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism() = default;

	bool Initialize(HINSTANCE);
	int Run();

	inline WindowController& GetWindowController() { return mWindowController; };
	inline ConsoleController& GetConsoleController() { return mConsoleController; };
	inline D3DController& GetD3DController() { return mD3DController; };
	inline SceneManager& GetSceneManager() { return mSceneManager; };

private:

	WindowController mWindowController;
	ConsoleController mConsoleController;
	D3DController mD3DController;
	SceneManager mSceneManager;


};
