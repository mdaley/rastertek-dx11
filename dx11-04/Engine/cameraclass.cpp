#include "CameraClass.h"

void* CameraClass::operator new (size_t size)
{
    void* p = _aligned_malloc(size, 16);
    if (p == 0)
    {
        throw std::bad_alloc();
    }
    
    return p;
}

void CameraClass::operator delete(void* p)
{
    CameraClass* d = static_cast<CameraClass*>(p);
    _aligned_free(d);
}

CameraClass::CameraClass()
{
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_position.z = 0.0f;
    m_rotation.x = 0.0f;
    m_rotation.y = 0.0f;
    m_rotation.z = 0.0f;
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(XMFLOAT3 position)
{
    m_position = position;
}

void CameraClass::SetRotation(XMFLOAT3 rotation)
{
    m_rotation = rotation;
}

XMFLOAT3 CameraClass::GetPosition()
{
    return m_position;
}

XMFLOAT3 CameraClass::GetRotation()
{
    return m_rotation;
}

void CameraClass::Render()
{
    float yaw, pitch, roll;

    // Setup the vector that points upwards.
    XMVECTORF32 up_f = { 0.0f, 1.0f, 0.0f, 0.0f };
    XMVECTOR up = up_f;

    // Setup the position of the camera in the world.
    XMVECTORF32 position_f = { m_position.x, m_position.y, m_position.z, 0.0f };
    XMVECTOR position = position_f;

    // Setup where the camera is looking by default.
    XMVECTORF32 lookAt_f = { 0.0f, 0.0f, 1.0f, 0.0f };
    XMVECTOR lookAt = lookAt_f;

    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    pitch = m_rotation.x * 0.0174532925f;
    yaw = m_rotation.y * 0.0174532925f;
    roll = m_rotation.z * 0.0174532925f;

    // Create the rotation matrix from the yaw, pitch, and roll values.
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
    up = XMVector3TransformCoord(up, rotationMatrix);
    //D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
    //D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

    // Translate the rotated camera position to the location of the viewer.
    lookAt = position + lookAt;

    // Finally create the view matrix from the three updated vectors.
    m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
    //D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(XMMATRIX& view)
{
    view = m_viewMatrix;
}