#include "d3dclass.h"

void* D3DClass::operator new (size_t size)
{
    void* p = _aligned_malloc(size, 16);
    if (p == 0)
    {
        throw std::bad_alloc();
    }

    return p;
}

void D3DClass::operator delete(void* p)
{
    D3DClass* d = static_cast<D3DClass*>(p);
    _aligned_free(d);
}

D3DClass::D3DClass()
{
}

/*D3DClass::D3DClass(const D3DClass& other)
{
}*/

D3DClass::~D3DClass()
{
}

void D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{    
    // Store the vsync setting.
    m_vsync_enabled = vsync;

    // Get DirectX graphics interface factory.
    auto factory = GetIDXGIFactory();

    // Use the factory to create an adapter for the primary graphics interface (video card).
    auto adapter = GetPrimaryDisplayAdapter(factory);

    // Obtain the primary adapter output, i.e. the main monitor.
    auto monitor = GetMonitorForAdapter(0, adapter);

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the monitor.
    unsigned int numModes;
    auto displayModeList = GetDisplayModesForMonitor(monitor, numModes);
    
    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    unsigned int numerator, denominator;
    GetRefreshRateForWindowSize(numModes, displayModeList, screenWidth, screenHeight, numerator, denominator);

    GetVideoCardInformation(adapter);

    std::stringstream oss;
    oss << "Display Adapter = " << m_videoCardDescription << "\n";
    oss << "Memory = " << m_videoCardMemory << "MB" << "\n";
    oss << "Refresh Rate = " << numerator << " / " << denominator << "\n";
    OutputDebugStringA(oss.str().c_str());

    DXGI_SWAP_CHAIN_DESC swapChainDesc = SetSwapChainDescription(screenWidth, screenHeight, numerator, denominator, hwnd, fullscreen);

    // Create the swap chain, device and device context member variables.
    CreateSwapChainDeviceAndContext(swapChainDesc);

    CreateRenderTargetView();

    CreateDepthBuffer(screenWidth, screenHeight);

    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = SetDepthStencilDescription();

    // Create the depth stencil state.
    CreateDepthStencilState(depthStencilDesc);

    // Initialize the depth stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = CreateDepthStencilViewDescription();

    // Create the depth stencil view.
    CreateDepthStencilView(depthStencilViewDesc);

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    ID3D11RenderTargetView* renderTargetView_unsafe = m_renderTargetView.get();
    m_deviceContext->OMSetRenderTargets(1, &renderTargetView_unsafe, m_depthStencilView.get());

    // Setup the raster description which will determine how and what polygons will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc = SetRasterizerDescription();

    // Create the rasterizer state based on description.
    SetRasterizerState(rasterDesc);

    // Setup the viewport for rendering.
    D3D11_VIEWPORT viewport;
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Setup the projection matrix.
    float fieldOfView = (float)XM_PI / 4.0f;
    float screenAspect = (float)screenWidth / (float)screenHeight;

    // Create the projection matrix for 3D rendering.
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    // Initialize the world matrix to the identity matrix.
    m_worldMatrix = XMMatrixIdentity();

    // Create an orthographic projection matrix for 2D rendering.
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
}


void D3DClass::Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.get(), color);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void D3DClass::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsync_enabled)
    {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    }
    else
    {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }
}


ID3D11Device* D3DClass::GetDevice()
{
    return m_device.get();
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext.get();
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

IDXGI_FACTORY_UNIQUE_PTR D3DClass::GetIDXGIFactory()
{
    IDXGIFactory* factory_unsafe;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Creation of DSXI Factory failed with result code = ") << result;
    }
    IDXGI_FACTORY_UNIQUE_PTR factory(factory_unsafe);

    return factory;
}

IDXGI_ADAPTER_UNIQUE_PTR D3DClass::GetPrimaryDisplayAdapter(const IDXGI_FACTORY_UNIQUE_PTR& factory)
{
    IDXGIAdapter* adapter_unsafe;
    HRESULT result = factory->EnumAdapters(0, &adapter_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Creation of DXGI adapter failed with result code = ") << result;
    }
    IDXGI_ADAPTER_UNIQUE_PTR adapter(adapter_unsafe);

    return adapter;
}

IDXGI_OUTPUT_UNIQUE_PTR D3DClass::GetMonitorForAdapter(UINT monitorNumber, const IDXGI_ADAPTER_UNIQUE_PTR& adapter)
{
    IDXGIOutput* adapterOutput_unsafe;
    HRESULT result = adapter->EnumOutputs(monitorNumber, &adapterOutput_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Getting monitor ") << monitorNumber << " for adapter failed with result code = " << result;
    }
    IDXGI_OUTPUT_UNIQUE_PTR adapterOutput(adapterOutput_unsafe);

    return adapterOutput;
}

unique_ptr<DXGI_MODE_DESC[]> D3DClass::GetDisplayModesForMonitor(const IDXGI_OUTPUT_UNIQUE_PTR& monitor, unsigned int& numModes)
{
    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    //unsigned int numModes;
    HRESULT result = monitor->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        throw engine_exception("Getting number of modes for monitor failed with result code = ") << result;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    unique_ptr<DXGI_MODE_DESC[]> displayModeList(new DXGI_MODE_DESC[numModes]);

    // Now fill the display mode list structures.
    result = monitor->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.get());
    if (FAILED(result))
    {
        throw engine_exception("Getting disply mode list failed with result code = ") << result;
    }

    return displayModeList;
}

void D3DClass::GetVideoCardInformation(const IDXGI_ADAPTER_UNIQUE_PTR& adapter)
{
    // Get the adapter (video card) description.
    DXGI_ADAPTER_DESC adapterDesc;
    HRESULT result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        throw engine_exception("Couldn't get adapter description from adapter");
    }

    // Store the dedicated video card memory in megabytes.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    unsigned int stringLength;
    int error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
    {
        throw engine_exception("Couldn't write adapter description to char array");
    }
}

void D3DClass::GetRefreshRateForWindowSize(const unsigned int numModes, const unique_ptr<DXGI_MODE_DESC[]>& displayModeList,
    const unsigned int screenWidth, const unsigned int screenHeight,
    unsigned int& numerator, unsigned int& denominator)
{
    for (unsigned int i = 0; i < numModes; i++)
    {
        stringstream oss;
        oss << "Display Mode: " << displayModeList[i].Width << ", " << displayModeList[i].Height << "\n";
        OutputDebugStringA(oss.str().c_str());

        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }
}

DXGI_SWAP_CHAIN_DESC D3DClass::SetSwapChainDescription(int unsigned screenWidth, int unsigned screenHeight, unsigned int numerator,
    unsigned int denominator, HWND hwnd, bool fullscreen)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    return swapChainDesc;
}

void D3DClass::CreateSwapChainDeviceAndContext(const DXGI_SWAP_CHAIN_DESC& swapChainDesc)
{
    // Set the feature level to DirectX 11.

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    IDXGISwapChain* swapChain_unsafe;
    ID3D11Device* device_unsafe;
    ID3D11DeviceContext* deviceContext_unsafe;
    // Create the swap chain, Direct3D device, and Direct3D device context.
    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &swapChain_unsafe, &device_unsafe, NULL, &deviceContext_unsafe);

    if (FAILED(result))
    {
        throw engine_exception("Could not create swap chain, device and device context, result code = ") << result;
    }

    m_swapChain = IDXGI_SWAP_CHAIN_UNIQUE_PTR(swapChain_unsafe);
    m_device = ID3D11_DEVICE_UNIQUE_PTR(device_unsafe);
    m_deviceContext = ID3D11_DEVICE_CONTEXT_UNIQUE_PTR(deviceContext_unsafe);
}

void D3DClass::CreateRenderTargetView()
{
    // Get the pointer to the back buffer.
    unique_ptr<ID3D11Texture2D, ReleaseResource<ID3D11Texture2D>> backBufferPtr;
    HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        throw engine_exception("Could not obtain back buffer pointer, result code = ") << result;
    }

    // Create the render target view with the back buffer pointer.
    ID3D11RenderTargetView* renderTargetView_unsafe;
    result = m_device->CreateRenderTargetView(backBufferPtr.get(), NULL, &renderTargetView_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Could not create render target view, result code = ") << result;
    }

    m_renderTargetView = ID3D11_RENDER_TARGET_VIEW_UNIQUE_PTR(renderTargetView_unsafe);
}

void D3DClass::CreateDepthBuffer(const unsigned int screenWidth, const unsigned int screenHeight)
{
    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    ID3D11Texture2D* depthStencilBuffer_unsafe;
    HRESULT result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Could not create depth buffer, result code = ") << result;
    }
    m_depthStencilBuffer = ID3D11_TEXTURE_2D_UNIQUE_PTR(depthStencilBuffer_unsafe);
}

D3D11_DEPTH_STENCIL_DESC D3DClass::SetDepthStencilDescription()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    return depthStencilDesc;
}

void D3DClass::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& depthStencilDesc)
{
    ID3D11DepthStencilState* depthStencilState_unsafe;
    HRESULT result = m_device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Could not create depth stencil state, result code = ") << result;
    }
    m_depthStencilState = ID3D11_DEPTH_STENCIL_STATE_UNIQUE_PTR(depthStencilState_unsafe);

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState.get(), 1);
}

D3D11_DEPTH_STENCIL_VIEW_DESC D3DClass::CreateDepthStencilViewDescription()
{
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    return depthStencilViewDesc;
}

void D3DClass::CreateDepthStencilView(D3D11_DEPTH_STENCIL_VIEW_DESC& depthStencilViewDesc)
{
    ID3D11DepthStencilView* depthStencilView_unsafe;
    HRESULT result = m_device->CreateDepthStencilView(m_depthStencilBuffer.get(), &depthStencilViewDesc, &depthStencilView_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Couldn't create depth stencil view, result code = ") << result;
    }
    m_depthStencilView = ID3D11_DEPTH_STENCIL_VIEW_UNIQUE_PTR(depthStencilView_unsafe);
}

D3D11_RASTERIZER_DESC D3DClass::SetRasterizerDescription()
{
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    return rasterDesc;
}

void D3DClass::SetRasterizerState(D3D11_RASTERIZER_DESC& rasterDesc)
{
    ID3D11RasterizerState* rasterState_unsafe;
    HRESULT result = m_device->CreateRasterizerState(&rasterDesc, &rasterState_unsafe);
    if (FAILED(result))
    {
        throw engine_exception("Could not set rasterizer state, result code = ") << result;
    }
    m_rasterState = ID3D11_RASTERIZER_STATE_UNIQUE_PTR(rasterState_unsafe);

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState.get());
}