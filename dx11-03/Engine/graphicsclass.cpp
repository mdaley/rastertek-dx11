#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
}

/*GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}*/

GraphicsClass::~GraphicsClass()
{
}

void GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    m_D3D = unique_ptr<D3DClass>(new D3DClass());
    m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
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
    m_D3D->EndScene();
    return true;
}