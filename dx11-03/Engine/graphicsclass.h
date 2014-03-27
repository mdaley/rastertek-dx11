#pragma once

#include "engine.h"
#include "d3dclass.h"

using namespace std;

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
    GraphicsClass();
    //GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    void Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render();
    unique_ptr<D3DClass> m_D3D;
};
