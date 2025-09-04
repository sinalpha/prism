#pragma once

#include <assert.h>

#include <Windows.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "DescriptorHeapAllocator.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static DescriptorHeapAllocator* gDescriptorHeapAllocator;

class Prism;
class WindowController {
public:

	WindowController() = default;
	~WindowController();

	static LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	void Init(HINSTANCE, Prism*);
	
	void InitImGui();
	void StartImGuiFrame();

	int GetClientWidth();
	int GetClientHeight();
	float GetAspect();
	HWND GetWindowHandler();

private:



	Prism* mPrism{ nullptr };

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