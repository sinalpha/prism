#pragma once

#include <Windows.h>

class Prism {
public:


	Prism(HINSTANCE pHInstance);
	Prism(const Prism&) = delete;
	Prism& operator=(const Prism&) = delete;
	~Prism();


	bool initialize();
	static LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	int run();


protected:


	bool initWindow();


	// variable for setting window;
	HINSTANCE hInstance{ NULL };
	WNDCLASSEX windowClass{ NULL };
	HWND hWindow{ NULL };
	LPCWSTR	windowTitle{ L"Prism" };
	int clientWidth{ 1280 };
	int clientHeight{ 720 };
	int displayWidth{ GetSystemMetrics(SM_CXSCREEN) };
	int displayHeight{ GetSystemMetrics(SM_CYSCREEN) };
	int clientLeft{ (displayWidth - clientWidth) / 2 };
	int clientTop{ (displayHeight - clientHeight) / 2 };


};