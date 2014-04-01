#pragma once
#include "engine.h"
#include "engine_exception.h"
#include "wrl/client.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

// Allows the releasing of a resource that uses the Release() method (e.g. IDXGI and ID3D resources).
// Useful if you're using a unique_ptr to manage a COM object instead of ComPtr.
template <class T>
class ReleaseResource
{
public:
    void operator()(T *s) const
    {
        stringstream ss;
        ss << "Releasing " << typeid(T).name() << " instance\n";
        OutputDebugStringA(ss.str().c_str());
        s->Release();
    }
};

#define IDXGI_FACTORY_COM_PTR ComPtr<IDXGIFactory>
#define IDXGI_ADAPTER_COM_PTR ComPtr<IDXGIAdapter>
#define IDXGI_OUTPUT_COM_PTR ComPtr<IDXGIOutput>
#define IDXGI_SWAP_CHAIN_COM_PTR ComPtr<IDXGISwapChain>
#define ID3D11_DEVICE_COM_PTR ComPtr<ID3D11Device>
#define ID3D11_DEVICE_CONTEXT_COM_PTR ComPtr<ID3D11DeviceContext>
#define ID3D11_RENDER_TARGET_VIEW_COM_PTR ComPtr<ID3D11RenderTargetView>
#define ID3D11_TEXTURE_2D_COM_PTR ComPtr<ID3D11Texture2D>
#define ID3D11_DEPTH_STENCIL_STATE_COM_PTR ComPtr<ID3D11DepthStencilState>
#define ID3D11_DEPTH_STENCIL_VIEW_COM_PTR ComPtr<ID3D11DepthStencilView>
#define ID3D11_RASTERIZER_STATE_COM_PTR ComPtr<ID3D11RasterizerState>

class D3DClass
{
public:
    D3DClass();
    ~D3DClass();

    void Initialize(const int screenWidth, const int screenHeight, const bool vsync, const HWND hwnd,
                    const bool fullscreen, const float screenDepth, const float screenNear);

    void Shutdown();

    void BeginScene(float, float, float, float);

    void EndScene();

    // Return raw pointer for use outsie the object (but don't try to manage these pointers).
    ID3D11Device* GetDevice();

    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX& projection);

    void GetWorldMatrix(XMMATRIX& world);

    void GetOrthoMatrix(XMMATRIX& ortho);

    void GetVideoCardInfo(char* name, int& mbMemory);

    // Operators for new and delete needed to set 16-byte alignment.
    static void* operator new (size_t size);

    static void operator delete (void* p);

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGI_SWAP_CHAIN_COM_PTR m_swapChain;
    ID3D11_DEVICE_COM_PTR m_device;
    ID3D11_DEVICE_CONTEXT_COM_PTR m_deviceContext;
    ID3D11_RENDER_TARGET_VIEW_COM_PTR m_renderTargetView;
    ID3D11_TEXTURE_2D_COM_PTR m_depthStencilBuffer;
    ID3D11_DEPTH_STENCIL_STATE_COM_PTR m_depthStencilState;
    ID3D11_DEPTH_STENCIL_VIEW_COM_PTR m_depthStencilView;
    ID3D11_RASTERIZER_STATE_COM_PTR m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;

    IDXGI_FACTORY_COM_PTR GetIDXGIFactory();

    IDXGI_ADAPTER_COM_PTR GetPrimaryDisplayAdapter(const IDXGI_FACTORY_COM_PTR& factory);

    IDXGI_OUTPUT_COM_PTR GetMonitorForAdapter(const unsigned int monitorNumber, const IDXGI_ADAPTER_COM_PTR& adapter);

    unique_ptr<DXGI_MODE_DESC[]> GetDisplayModesForMonitor(const IDXGI_OUTPUT_COM_PTR& monitor, unsigned int& numModes);

    void D3DClass::GetRefreshRateForWindowSize(const unsigned int numModes, const unique_ptr<DXGI_MODE_DESC[]>& displayModeList,
                                               const unsigned int screenWidth, const unsigned int screenHeight,
                                               unsigned int& numerator, unsigned int& denominator);

    void GetVideoCardInformation(const IDXGI_ADAPTER_COM_PTR& adapter);

    DXGI_SWAP_CHAIN_DESC SetSwapChainDescription(const unsigned int screenWidth, const unsigned int screenHeight, const unsigned int numerator,
                                                 const unsigned int denominator, const HWND hwnd, const bool fullscreen);

    void CreateSwapChainDeviceAndContext(const DXGI_SWAP_CHAIN_DESC& swapChainDesc);

    void CreateRenderTargetView();

    void CreateDepthBuffer(const unsigned int screenWidth, const unsigned int screenHeight);

    D3D11_DEPTH_STENCIL_DESC D3DClass::SetDepthStencilDescription();

    void CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& depthStencilDesc);

    D3D11_DEPTH_STENCIL_VIEW_DESC CreateDepthStencilViewDescription();

    void CreateDepthStencilView(D3D11_DEPTH_STENCIL_VIEW_DESC& depthStencilViewDesc);

    D3D11_RASTERIZER_DESC SetRasterizerDescription();

    void SetRasterizerState(D3D11_RASTERIZER_DESC& rasterDesc);
};