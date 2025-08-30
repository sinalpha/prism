#pragma once

#include <Windows.h>

#include "WindowController.h"
#include "ConsoleController.h"
#include "D3DController.h"
#include "Scene.h"



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
	inline Scene* GetCurrentScene() { return mCurrentScene; };

private:

	WindowController mWindowController;
	ConsoleController mConsoleController;
	D3DController mD3DController;
	
	Scene* mCurrentScene;


};
