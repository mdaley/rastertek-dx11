#include "graphicsclass.h"

/*void* GraphicsClass::operator new (size_t size)
{
    void* p = _aligned_malloc(size, 16);
    if (p == 0)
    {
        throw std::bad_alloc();
    }

    return p;
}

void GraphicsClass::operator delete(void* p)
{
    GraphicsClass* d = static_cast<GraphicsClass*>(p);
    _aligned_free(d);
}*/

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::~GraphicsClass()
{
}

void GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    m_D3D = unique_ptr<D3DClass>(new D3DClass());
    m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    m_Camera = unique_ptr<CameraClass>(new CameraClass());
    m_Camera->SetPosition({ 0.0f, 0.0f, -10.0f });
    m_Model = unique_ptr<ModelClass>(new ModelClass());
    m_Model->Initialize(m_D3D->GetDevice());
    m_ColorShader = unique_ptr<ColorShaderClass>(new ColorShaderClass());
    m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
}

void GraphicsClass::Shutdown()
{
    return;
}

bool GraphicsClass::Frame()
{
    return Render();
}

bool GraphicsClass::Render()
{
    m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

    m_Camera->Render();

    XMMATRIX view, world, projection;
    m_Camera->GetViewMatrix(view);
    m_D3D->GetWorldMatrix(world);
    m_D3D->GetProjectionMatrix(projection);

    m_Model->Render(m_D3D->GetDeviceContext());

    m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), world, view, projection);

    m_D3D->EndScene();
    return true;
}