#pragma once

#include <assert.h>

#include <Windows.h>

class WindowManager {
public:

	WindowManager() = default;
	~WindowManager() = default;

	static LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	void Init(HINSTANCE);

	int GetClientWidth();
	int GetClientHeight();
	float GetAspect();
	HWND GetWindowHandler();

private:

	HINSTANCE mHInstance{ NULL };
	WNDCLASSEX mWindowClass{ NULL };
	HWND mHWindow{ NULL };
	LPCWSTR	mWindowTitle{ L"prism" };
	int mClientWidth{ 1280 };
	int mClientHeight{ 720 };
	int mDisplayWidth{ GetSystemMetrics(SM_CXSCREEN) };
	int mDisplayHeight{ GetSystemMetrics(SM_CYSCREEN) };
	int mClientLeft{ (mDisplayWidth - mClientWidth) / 2 };
	int mClientTop{ (mDisplayHeight - mClientHeight) / 2 };
	float mAspect{ static_cast<float> (mClientWidth / static_cast<float>(mClientHeight)) };

};