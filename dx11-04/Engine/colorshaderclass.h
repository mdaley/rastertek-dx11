#pragma once
#include "engine.h"
#include "D3DCompiler.h"
#include <fstream>
#include <vector>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

class ColorShaderClass
{
public:
    ColorShaderClass();

    ~ColorShaderClass();

    void Initialize(ID3D11Device* device, HWND hwnd);

    void Render(ID3D11DeviceContext* deviceContext, const int indexCount, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_layout;
    ComPtr<ID3D11Buffer> m_matrixBuffer;

    void InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName);

    void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

    unique_ptr<byte[]> LoadFile(WCHAR* fileName, unsigned int& numBytes);
};

