#include "Prism.h"


Prism::Prism(HINSTANCE pHInstance) : hInstance{ pHInstance } {

}

Prism::~Prism() {

}

bool Prism::initialize() {


	if(!initWindow()) {
        MessageBox(nullptr, L"er", L"HR Failed", MB_OK);
		return false;
	}

    if (!initDx3D()) {
        MessageBox(nullptr, L"Failed to initialize Direct3D 12.", L"Error", MB_OK);
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

    createPipeLine();
    
	Model model(device, commandList);
	model.LoadModel("assets\\cottage_fbx.fbx");

    /*set canera pos*/
    XMVECTOR pos = XMVectorSet(100.f, 100.f, 100.f, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    //store view matrix to model1
    XMStoreFloat4x4(&model.GetWorld(), view);


    XMMATRIX world = XMLoadFloat4x4(&model.GetWorld());
    XMMATRIX proj = XMLoadFloat4x4(&model.GetProj());
    XMMATRIX worldViewProj = world * view * proj;

    while (msg.message != WM_QUIT)
    {

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        else {

            clearBackBuffer();
            

            commandList->SetPipelineState(pipelineState.Get());
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);
            commandList->SetGraphicsRootSignature(rootSignature.Get());

            D3D12_VERTEX_BUFFER_VIEW vertexBufViews[] = {
                model.GetVertexBufferView(),
            };
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, _countof(vertexBufViews), vertexBufViews);

            commandList->DrawInstanced(model.GetVerticesNum(), 1, 0, 0);

            presentBackBuffer();
        }
    }

    return (int)msg.wParam;

}

void Prism::getHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}

bool Prism::initWindow() {
	
	// Set up the window class
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

	// Calculate the window size based on the client area size
    RECT windowRect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&windowRect, WS_POPUP, FALSE);
    int windowLeft = clientLeft + windowRect.left;
    int windowTop = clientTop + windowRect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
	// Create the window
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

bool Prism::initDx3D() {
       
    UINT dxgiFactoryFlags = 0;
    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (!initDeviceAndCommandInterfaces(factory)) {
        MessageBox(nullptr, L"Failed to initialize device and command interfaces.", L"Error", MB_OK);
        return false;
	}

    if (!initFence()) {
        MessageBox(nullptr, L"Failed to initialize fence.", L"Error", MB_OK);
        return false;
    }

    if (!initBackBuffer(factory)) {
        MessageBox(nullptr, L"Failed to initialize back buffer.", L"Error", MB_OK);
        return false;
	}

    return true;
}

bool Prism::initDeviceAndCommandInterfaces(ComPtr<IDXGIFactory4>& factory) {

    if (useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        getHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&device)
        ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));

    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr, IID_PPV_ARGS(&commandList)));


    return true;
}

bool Prism::initFence() {

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    if (nullptr == (fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS))) {

        MessageBox(nullptr, L"err", L"Failed to create Fence objects", MB_OK);
        return false;

    }
    fenceValue = 1;

    return true;
}

bool Prism::initBackBuffer(ComPtr<IDXGIFactory4>& factory) {

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.Width = clientWidth;
    swapChainDesc.Height = clientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> initSwapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        commandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
        hWindow,
        &swapChainDesc,
        nullptr,
        nullptr,
        &initSwapChain
    ));

    ThrowIfFailed(initSwapChain.As(&swapChain));
    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = frameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&bbvHeap)));

        bbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(bbvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < frameCount; ++n)
        {
            ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
            device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, bbvDescriptorSize);
        }
    }

    return true;

}

void Prism::createPipeLine() {
    
 
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

 
    ID3DBlob* blob;
    D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);

    device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature.GetAddressOf()));
    assert(SUCCEEDED(Hr));
    blob->Release();
 


    ShaderReader vs("assets\\VertexShader.cso");
    assert(vs.succeeded());
    ShaderReader ps("assets\\PixelShader.cso");
    assert(ps.succeeded());


    UINT slot0 = 0;
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.ForcedSampleCount = 0;
    rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = false;
    blendDesc.RenderTarget[0].LogicOpEnable = false;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = false;



    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature = rootSignature.Get();
    pipelineDesc.VS = { vs.code(), vs.size() };
    pipelineDesc.PS = { ps.code(), ps.size() };
    pipelineDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    pipelineDesc.RasterizerState = rasterDesc;
    pipelineDesc.BlendState = blendDesc;
    pipelineDesc.DepthStencilState = depthStencilDesc;
    pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineDesc.SampleMask = UINT_MAX;
    pipelineDesc.SampleDesc.Count = 1;
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineDesc.NumRenderTargets = 1;
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateGraphicsPipelineState(
        &pipelineDesc,
        IID_PPV_ARGS(pipelineState.GetAddressOf())
    );


    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = clientWidth;
    viewport.Height = (FLOAT)clientHeight;
    viewport.MinDepth = (FLOAT)0.0f;
    viewport.MaxDepth = 1.0f;


    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = clientWidth;
    scissorRect.bottom = clientHeight;
}

void Prism::clearBackBuffer() {


    frameIndex = swapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

 
    auto hBbvHeap = bbvHeap->GetCPUDescriptorHandleForHeapStart();
    hBbvHeap.ptr += frameIndex * bbvDescriptorSize;
    commandList->OMSetRenderTargets(1, &hBbvHeap, false, nullptr);

 
    const float clearColor[] = { .25f, .25f, .25f, 1.0f };
    commandList->ClearRenderTargetView(hBbvHeap, clearColor, 0, nullptr);

}

void Prism::presentBackBuffer() {
    
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = renderTargets[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();

    ID3D12CommandList* commandLists[] = { commandList.Get()};
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    waitDrawDone();

  
    swapChain->Present(1, 0);

    Hr = commandAllocator.Get()->Reset();

    Hr = commandList->Reset(commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(Hr));

}

void Prism::waitDrawDone() {

    //fvale가 코맨드 종료 후에 팬스에 기록되도록 한다.
	UINT64 fvalue = fenceValue;
	commandQueue->Signal(fence.Get(), fvalue);
    fenceValue++;

    //아직 커맨드큐가 종료되지 않음을 확인한다.
    if (fence->GetCompletedValue() < fvalue) {
        //fence가 fvalue값이 되었다면 종료를 알리는 이벤트를 생성한다.
        fence->SetEventOnCompletion(fvalue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
	}


}


