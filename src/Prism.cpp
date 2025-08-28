#include "Prism.h"

bool Prism::Initialize(HINSTANCE pHInstance) {

    mWindowController.Init(pHInstance);
    mConsoleController.Init();


    if (!InitDx3D()) {
        MessageBox(nullptr, L"Failed to initialize Direct3D 12.", L"Error", MB_OK);
		return false;
    }

    if (!InitScenes()){
		MessageBox(nullptr, L"Failed to initialize scenes.", L"Error", MB_OK);
        return false;
    }

	return true;
    
}

int Prism::Run() {


    MSG msg = { 0 };
    
    while (msg.message != WM_QUIT)
    {

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        else {

            Update();
            Render();

        }
    }

    return (int)msg.wParam;

}

void Prism::Update() {

    //回転用ラジアン
    static float radian = 0;
    radian += 0.01f;
    //ワールドマトリックス
    XMMATRIX world = XMMatrixRotationY(radian);
    //ビューマトリックス
    XMVECTOR eye = { 15.f, 0, 0.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
    XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
    //プロジェクションマトリックス
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mWindowController.GetAspect(), 1.0f, 100.0f);
    //コンスタントバッファ０更新
    XMMATRIX worldViewProj = world * view * proj;

    mModel.SetMat(worldViewProj);

}

void Prism::Render() {

    CreatePipeLine();

    SetCommandList();

}

void Prism::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
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

bool Prism::InItConsole() {

    AllocConsole();
    
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    return true;

}

bool Prism::InitDx3D() {
       

    if (!InitFactory()) {
        MessageBox(nullptr, L"Failed to initialize DXGI factory.", L"Error", MB_OK);
		return false;
    }

    if (!InitDevice()) {
        MessageBox(nullptr, L"Failed to initialize device.", L"Error", MB_OK);
        return false;
	}

    if (!InitCommandInterfaces()) {
        MessageBox(nullptr, L"Failed to initialize command interfaces.", L"Error", MB_OK);
        return false;
    }

    if (!InitFence()) {
        MessageBox(nullptr, L"Failed to initialize fence.", L"Error", MB_OK);
        return false;
    }

    if (!InitFrameInterfaces()) {
        MessageBox(nullptr, L"Failed to initialize back buffer.", L"Error", MB_OK);
        return false;
	}

    return true;
}

bool Prism::InitScenes() {

    mModel = Model(mDevice, mCommandList);
    mModel.LoadModel("assets\\cottage_fbx.fbx");

    return true;

}

bool Prism::InitFactory() {


    UINT dxgiFactoryFlags = 0;

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mFactory)));

    return true;

}

bool Prism::InitDevice() {

    if (scmUseWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(mFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(mFactory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }

    return true;

}

bool Prism::InitCommandInterfaces() {


    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

    ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

    ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        mCommandAllocator.Get(),
        nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));


    return true;
}

bool Prism::InitFence() {

    ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
    if (nullptr == (mFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS))) {

        MessageBox(nullptr, L"err", L"Failed to create Fence objects", MB_OK);
        return false;

    }
    mFenceValue = 1;

    return true;
}

bool Prism::InitFrameInterfaces() {


    if (!InitSwapChain()) {
            MessageBox(nullptr, L"Failed to initialize swap chain.", L"Error", MB_OK);
			return false;
    }

    if (!InitRenderTargetViewHeap()){
        MessageBox(nullptr, L"Failed to initialize render target view heap.", L"Error", MB_OK);
        return false;
	}


    if (!InitRenderTarget()) {
        MessageBox(nullptr, L"Failed to initialize render target.", L"Error", MB_OK);
		return false;
    }

    return true;

}

bool Prism::InitSwapChain() {

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = scmFrameCount;
    swapChainDesc.Width = mWindowController.GetClientWidth();
    swapChainDesc.Height = mWindowController.GetClientHeight();
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> initSwapChain;
    ThrowIfFailed(mFactory->CreateSwapChainForHwnd(
        mCommandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
        mWindowController.GetWindowHandler(),
        &swapChainDesc,
        nullptr,
        nullptr,
        &initSwapChain
    ));

    ThrowIfFailed(initSwapChain.As(&mSwapChain));
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    return true;
}

bool Prism::InitRenderTargetViewHeap() {

    // Create descriptor heaps.
    
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = scmFrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mBackBufferViewHeap)));

    mBackBufferViewDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    return true;

}

bool Prism::InitRenderTarget() {

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mBackBufferViewHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT n = 0; n < scmFrameCount; ++n)
    {
        ThrowIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
        mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, mBackBufferViewDescriptorSize);
    }

    return true;

}

void Prism::CreatePipeLine() {
    
    //루트 시그네이처 설정
    D3D12_DESCRIPTOR_RANGE  range[1] = {};
    UINT b0 = 0;
    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    range[0].BaseShaderRegister = b0;
    range[0].NumDescriptors = 1;
    range[0].RegisterSpace = 0;
    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParam[1] = {};
    rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParam[0].DescriptorTable.pDescriptorRanges = range;
    rootParam[0].DescriptorTable.NumDescriptorRanges = _countof(range);
    rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.pParameters = rootParam;
    desc.NumParameters = _countof(rootParam);

    ID3DBlob* blob;
    D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);

    mDevice->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
        IID_PPV_ARGS(mRootSignature.GetAddressOf()));
    assert(SUCCEEDED(Hr));
    blob->Release();
 
    //쉐이더 읽기
    ShaderReader vs("assets\\VertexShader.cso");
    assert(vs.succeeded());
    ShaderReader ps("assets\\PixelShader.cso");
    assert(ps.succeeded());
    UINT slot0 = 0;
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    //레스터 라이저 설정
    D3D12_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.ForcedSampleCount = 0;
    rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    //블렌드 스테이트 설정
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

	//깊이버퍼 설정
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = false;

	//파이프라인 설정
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature = mRootSignature.Get();
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
    mDevice->CreateGraphicsPipelineState(
        &pipelineDesc,
        IID_PPV_ARGS(mPipelineState.GetAddressOf())
    );


	SetViewPort();
	SetScissorRect();

}

void Prism::SetViewPort() {
    mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width = (FLOAT)mWindowController.GetClientWidth();
    mViewport.Height = (FLOAT)mWindowController.GetClientHeight();
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
}

void Prism::SetScissorRect() {
    mScissorRect.left = 0;
    mScissorRect.top = 0;
    mScissorRect.right = mWindowController.GetClientWidth();
    mScissorRect.bottom = mWindowController.GetClientHeight();
}

void Prism::ClearBackBuffer() {


    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    mCommandList->ResourceBarrier(1, &barrier);

 
    auto hBbvHeap = mBackBufferViewHeap->GetCPUDescriptorHandleForHeapStart();
    hBbvHeap.ptr += mFrameIndex * mBackBufferViewDescriptorSize;
    mCommandList->OMSetRenderTargets(1, &hBbvHeap, false, nullptr);

 
    const float clearColor[] = { .25f, .25f, .25f, 1.0f };
    mCommandList->ClearRenderTargetView(hBbvHeap, clearColor, 0, nullptr);

}

void Prism::PresentBackBuffer() {
    
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    mCommandList->ResourceBarrier(1, &barrier);

    mCommandList->Close();

    ID3D12CommandList* commandLists[] = { mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    WaitDrawDone();

  
    mSwapChain->Present(1, 0);

    mCommandAllocator.Get()->Reset();

    mCommandList->Reset(mCommandAllocator.Get(), nullptr);


}

void Prism::WaitDrawDone() {

    //fvale가 코맨드 종료 후에 팬스에 기록되도록 한다.
	UINT64 fvalue = mFenceValue;
	mCommandQueue->Signal(mFence.Get(), fvalue);
    mFenceValue++;

    //아직 커맨드큐가 종료되지 않음을 확인한다.
    if (mFence->GetCompletedValue() < fvalue) {
        //fence가 fvalue값이 되었다면 종료를 알리는 이벤트를 생성한다.
        mFence->SetEventOnCompletion(fvalue, mFenceEvent);
        WaitForSingleObject(mFenceEvent, INFINITE);
	}


}

void Prism::SetCommandList(){

    ClearBackBuffer();


    mCommandList->SetPipelineState(mPipelineState.Get());
    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);
    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D12_VERTEX_BUFFER_VIEW vertexBufViews[] = {
        mModel.GetVertexBufferView(),
    };
    mCommandList->IASetVertexBuffers(0, _countof(vertexBufViews), vertexBufViews);
    D3D12_INDEX_BUFFER_VIEW indexBufferView = mModel.GetIndexBufferView();
    mCommandList->IASetIndexBuffer(&indexBufferView);
    mCommandList->SetDescriptorHeaps(1, mModel.GetDescriptorHeapAddress());
    auto hCbvHeap = mModel.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
    mCommandList->SetGraphicsRootDescriptorTable(0, hCbvHeap);
    mCommandList->DrawIndexedInstanced(mModel.GetIndicesNum(), 1, 0, 0, 0);


    PresentBackBuffer();

}