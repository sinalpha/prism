#include "WindowController.h"

#include "Prism.h"


void WindowController::Init(HINSTANCE pH, Prism* pPrism) {

	mPrism = pPrism;

    bool result;

    // Set up the window class
    mWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    mWindowClass.lpszClassName = L"WINDOW_CLASS";
    mWindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    mWindowClass.hInstance = mHInstance;
    mWindowClass.lpfnWndProc = MsgProc;
    mWindowClass.hIcon = LoadIcon(NULL, IDC_ICON);
    mWindowClass.cbClsExtra = 0;
    mWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mWindowClass.cbSize = sizeof(WNDCLASSEX);

    result = RegisterClassEx(&mWindowClass);

    // Calculate the window size based on the client area size
    RECT windowRect = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&windowRect, WS_POPUP, FALSE);
    int windowLeft = mClientLeft + windowRect.left;
    int windowTop = mClientTop + windowRect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Create the window
    mHWindow = CreateWindowEx(
        NULL,
        L"WINDOW_CLASS",
        mWindowTitle,
        WS_POPUP,
        windowLeft,
        windowTop,
        windowWidth,
        windowHeight,
        NULL,
        NULL,
        mHInstance,
        NULL
    );

    ShowWindow(mHWindow, SW_SHOW);
    UpdateWindow(mHWindow);

}

LRESULT WindowController::MsgProc(HWND pHwnd, UINT pMsg, WPARAM wParam, LPARAM lParam) {


    switch (pMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(pHwnd, pMsg, wParam, lParam);
    }


}

int WindowController::GetClientWidth(){
    return mClientWidth;
};

int WindowController::GetClientHeight(){
	return mClientHeight;
};

float WindowController::GetAspect() {
    return mAspect; 
}

HWND WindowController::GetWindowHandler(){
	return mHWindow;
}