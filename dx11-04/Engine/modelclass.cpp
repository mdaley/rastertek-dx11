#include "modelclass.h"

using namespace std;

ModelClass::ModelClass()
{
}


ModelClass::~ModelClass()
{
}

void ModelClass::Initialize(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    
    m_vertexCount = 3;
    m_indexCount = 3;

    // Setup the vertex array.
    unique_ptr<VertexType[]> vertices(new VertexType[m_vertexCount]);
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // Setup the index array.
    unique_ptr<unsigned long[]> indices(new unsigned long[m_indexCount]);
    indices[0] = 0;  // Bottom left.
    indices[1] = 1;  // Top middle.
    indices[2] = 2;  // Bottom right.
    
    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices.get();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        throw engine_exception("Creation of buffer failed with result code = ") << result;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices.get();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        throw engine_exception("Creation of index buffer failed with result code = ") << result;
    }
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int ModelClass::GetIndexCount()
{
    return m_indexCount;
}
