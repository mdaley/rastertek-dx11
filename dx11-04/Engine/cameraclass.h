#pragma once
#include "engine.h"

using namespace DirectX;

class CameraClass
{
public:
    CameraClass();
    ~CameraClass();

    void SetPosition(XMFLOAT3 position);
    void SetRotation(XMFLOAT3 rotation);

    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotation();

    void Render();
    void GetViewMatrix(XMMATRIX& view);

    // Operators for new and delete needed to set 16-byte alignment.
    static void* operator new (size_t size);

    static void operator delete (void* p);

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMMATRIX m_viewMatrix;
};

