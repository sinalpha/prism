#include "prism.h"

Prism::Prism(HINSTANCE pHInstance) : hInstance{ pHInstance } {

}

Prism::~Prism() {

}

bool Prism::initialize() {


	if(!initWindow()) {
        MessageBox(nullptr, L"er", L"HR Failed", MB_OK);
		return false;
	}

	return true;


}

LRESULT Prism::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {


    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }


}

int Prism::run() {


    MSG msg = { 0 };
    
    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;


}

bool Prism::initWindow() {
	
    
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = L"WINDOW_CLASS";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = MsgProc;
	windowClass.hIcon = LoadIcon(NULL, IDC_ICON);
	windowClass.cbClsExtra = 0;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.cbSize = sizeof(WNDCLASSEX);
	
	if (!RegisterClassEx(&windowClass)) {

        MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;

	}

    RECT windowRect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&windowRect, WS_POPUP, FALSE);
    int windowLeft = clientLeft + windowRect.left;
    int windowTop = clientTop + windowRect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
    hWindow = CreateWindowEx(
        NULL,
        L"WINDOW_CLASS",
        windowTitle,
        WS_POPUP,
        windowLeft,
        windowTop,
        windowWidth,
        windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWindow, SW_SHOW);
    UpdateWindow(hWindow);

    return true;


}
