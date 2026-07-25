// Graphics/Shader/Shader.cpp
#include "Shader.h"
#include "EngineHelper.h"

#include <d3dcompiler.h>
#include <fstream>


/* default */
////////////////////////////////////////////////

Shader::Shader() { } // Shader


void Shader::Bind(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // Bind


/* public */
////////////////////////////////////////////////

bool Shader::UpdateMatrixBuffer(ID3D11DeviceContext* context, const XMMATRIX& w, const XMMATRIX& v, const XMMATRIX& p)
{
    MatrixBuffer data;
    data.world = XMMatrixTranspose(w);
    data.view = XMMatrixTranspose(v);
    data.projection = XMMatrixTranspose(p);

    if (EngineHelper::SuccessCheck(
        m_matrixBuffer.Update(context, data),
        "m_matrixBuffer 업데이트 실패")
        == false) return false;

    m_matrixBuffer.BindVS(context, 0);

    return true;
} // UpdateMatrixBuffer


/* protected */
////////////////////////////////////////////////

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