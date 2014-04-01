#pragma once
#include "engine.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

class ModelClass
{
public:
    ModelClass();

    ~ModelClass();

    void Initialize(ID3D11Device* device);

    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();

private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    ComPtr<ID3D11Buffer> m_vertexBuffer, m_indexBuffer;
    int m_vertexCount, m_indexCount;
};

