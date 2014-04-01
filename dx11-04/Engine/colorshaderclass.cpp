#include "ColorShaderClass.h"


ColorShaderClass::ColorShaderClass()
{
}

ColorShaderClass::~ColorShaderClass()
{
}

void ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    InitializeShader(device, hwnd, L"E:\\workspace\\rastertek-dx11\\dx11-04\\Debug\\ColorVertexShader.cso", L"E:\\workspace\\rastertek-dx11\\dx11-04\\Debug\\ColorPixelShader.cso");
}

void ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName)
{
    ComPtr<ID3DBlob> vertexShaderBuffer;
    ComPtr<ID3DBlob> pixelShaderBuffer;

    unsigned int vertexShaderBytesSize;
    auto vertexShaderBytes = LoadFile(vertexShaderFileName, vertexShaderBytesSize);
    HRESULT result = device->CreateVertexShader(vertexShaderBytes.get(), vertexShaderBytesSize, nullptr, m_vertexShader.GetAddressOf());
    if (FAILED(result))
    {
        throw engine_exception("Couldn't create vertex shader, result code = ") << result;
    }

    unsigned int numBytes;
    auto pixelShaderBytes = LoadFile(pixelShaderFileName, numBytes);
    device->CreatePixelShader(pixelShaderBytes.get(), numBytes, nullptr, m_pixelShader.GetAddressOf());
    if (FAILED(result))
    {
        throw engine_exception("Couldn't create pixel shader, result code = ") << result;
    }

    // Now setup the layout of the data that goes into the shader.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    //result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
    //    vertexShaderBuffer->GetBufferSize(), &m_layout);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBytes.get(),
        vertexShaderBytesSize, &m_layout);
    if (FAILED(result))
    {
        throw engine_exception("Couldn'y create input layout, result code = ") << result;
    }

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, m_matrixBuffer.GetAddressOf());
    if (FAILED(result))
    {
        throw engine_exception("Couldn't create buffer, result code = ") << result;
    }
}

void ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, const int indexCount, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
{
    SetShaderParameters(deviceContext, world, view, projection);
    RenderShader(deviceContext, indexCount);
}

void ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
{
    // Transpose the matrices to prepare them for the shader.
    XMMATRIX worldMatrix = XMMatrixTranspose(world);
    XMMATRIX viewMatrix = XMMatrixTranspose(view);
    XMMATRIX projectionMatrix = XMMatrixTranspose(projection);

    // Lock the constant buffer so it can be written to.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        throw engine_exception("Couldn't lock constant buffer, result code = ") << result;
    }

    // Get a pointer to the data in the constant buffer.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer.Get(), 0);

    // Finally set the constant buffer as buffer zero in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout.Get());

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader.Get(), NULL, 0);
    deviceContext->PSSetShader(m_pixelShader.Get(), NULL, 0);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

unique_ptr<byte[]> ColorShaderClass::LoadFile(WCHAR* fileName, unsigned int& numBytes)
{
    ifstream file;
    file.open(fileName, ios::binary);

    if (!file.is_open())
    {
        throw engine_exception("Couldn't open file ") << fileName;
    }

    file.seekg(0, ios::end);
    int size = (int)file.tellg();
    file.seekg(0, ios::beg);

    unique_ptr<byte[]> bytes = unique_ptr<byte[]>(new byte[size]);
    
    file.read(reinterpret_cast<char*>(bytes.get()), size);

    numBytes = size;
    return bytes;
}
