#include "Pch.h"
#include "Shader.h"
// RenderingEngine
#include "Shader/ShaderBuffers.h"
// Common
#include "DebugHelper.h"
#include "ConstantHelper.h"
// etc
#include <d3dcompiler.h>
#include <fstream>

using namespace DirectX;
using namespace ConstantHelper;


Shader::Shader()
{
    m_type = ShaderType::None;
} // Shader


bool Shader::Compile(ID3D11Device* device,
    HWND hwnd,
    const std::wstring& path,
    LPCSTR entry,
    LPCSTR profile,
    ID3DBlob** blob)
{
    ID3DBlob* errorBlob = nullptr;

    HRESULT result = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry,
        profile,
        D3D10_SHADER_ENABLE_STRICTNESS,
        0,
        blob,
        &errorBlob
    );

    if (FAILED(result))
    {
        if (errorBlob)
            OutputError(errorBlob, hwnd, path);
        else 
            MessageBoxW(hwnd, path.c_str(), L"셰이더 파일이 없음", MB_OK);
        return false;
    }

    return true;
} // Compile


void Shader::OutputError(
    ID3DBlob* errorMsg,
    HWND hwnd,
    const std::wstring& path)
{
    char* compileErrors = (char*)(errorMsg->GetBufferPointer());
    unsigned long long bufferSize = errorMsg->GetBufferSize();

    std::ofstream fout("shader-error.txt");
    for (unsigned long long i = 0; i < bufferSize; i++)
        fout << compileErrors[i];
    fout.close();

    errorMsg->Release();

    MessageBoxW(hwnd, L"compiling shader 에러. shader-error.txt 확인 요망", path.c_str(), MB_OK);
} // OutputError


bool Shader::UpdateMatrixBuffer(ID3D11DeviceContext* context,
    XMMATRIX model, XMMATRIX view, XMMATRIX proj)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    MatrixBuffer* data = (MatrixBuffer*)mapped.pData;
    data->model = XMMatrixTranspose(model);
    data->view = XMMatrixTranspose(view);
    data->projection = XMMatrixTranspose(proj);

    XMMATRIX invVP = XMMatrixInverse(nullptr, view * proj);
    data->invViewProj = XMMatrixTranspose(invVP);

    context->Unmap(m_matrixBuffer.Get(), 0);
    return true;
} // UpdateMatrixBuffer


bool Shader::UpdateGlobalBuffer(ID3D11DeviceContext* context,
    float time, float frame, XMFLOAT3 cameraPos, XMMATRIX view, float fov, float aspect)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_globalBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    GlobalBuffer* data = (GlobalBuffer*)mapped.pData;
    data->iTime = time;
    data->iFrame = frame;
    data->iCameraPos = cameraPos;
    data->iResolution = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);
    data->fov = fov;
    data->aspect = aspect;

    XMVECTOR det;
    XMMATRIX invView = XMMatrixInverse(&det, view);
    XMStoreFloat3(&data->camRight, invView.r[0]);
    XMStoreFloat3(&data->camUp, invView.r[1]);
    XMStoreFloat3(&data->camForward, invView.r[2]);

    context->Unmap(m_globalBuffer.Get(), 0);
    return true;
} // UpdateGlobalBuffer
