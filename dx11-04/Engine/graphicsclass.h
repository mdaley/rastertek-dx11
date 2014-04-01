#pragma once

#include "engine.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"

using namespace std;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
    GraphicsClass();

    ~GraphicsClass();

    void Initialize(int screenWidth, int screenHeight, HWND hwnd);

    void Shutdown();

    bool Frame();

    // Operators for new and delete needed to set 16-byte alignment.
    //static void* operator new (size_t size);

    //static void operator delete (void* p);

private:
    bool Render();
    unique_ptr<D3DClass> m_D3D;
    unique_ptr<CameraClass> m_Camera;
    unique_ptr<ModelClass> m_Model;
    unique_ptr<ColorShaderClass> m_ColorShader;
};
