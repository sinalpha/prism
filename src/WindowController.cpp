#include "WindowController.h"

#include "Prism.h"

WindowController::~WindowController() {

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    DestroyWindow(mHWindow);
    UnregisterClass(L"WINDOW_CLASS", mHInstance);

}

//리펙토링 필요
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

LRESULT WindowController::MsgProc(HWND pHWnd, UINT pMsg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(pHWnd, pMsg, wParam, lParam))
        return true;


    switch (pMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(pHWnd, pMsg, wParam, lParam);
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

void WindowController::InitImGui() {

    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplWin32_Init(mHWindow);

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = mPrism->GetD3DController().GetDevicePtr().Get();
    init_info.CommandQueue = mPrism->GetD3DController().GetCommandQueuePtr().Get();
    init_info.NumFramesInFlight = mPrism->GetD3DController().csmFrameCount;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

	gDescriptorHeapAllocator = &mPrism->GetD3DController().GetDescriptorHeapAllocator();

    init_info.SrvDescriptorHeap = mPrism->GetD3DController().GetSRVDescriptorHeapForImGuiPtr().Get();
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return (*gDescriptorHeapAllocator).Alloc(out_cpu_handle, out_gpu_handle); };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return (*gDescriptorHeapAllocator).Free(cpu_handle, gpu_handle); };
    ImGui_ImplDX12_Init(&init_info);


}

void WindowController::StartImGuiFrame() {

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

}