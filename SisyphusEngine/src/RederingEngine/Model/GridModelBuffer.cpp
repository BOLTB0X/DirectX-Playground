#include "Pch.h"
#include "GridModelBuffer.h"
#include "DebugHelper.h"

using namespace DirectX;


GridModelBuffer::GridModelBuffer()
    : m_vertexCount(0),
    m_indexCount(0),
    m_gridWidth(0),
    m_gridHeight(0),
    m_gridScale(0.0f)
{
} // GridModelBuffer


GridModelBuffer::~GridModelBuffer()
{
    Shutdown();
} // GridModelBuffer


bool GridModelBuffer::Init(ID3D11Device* device, int width, int height, float scale)
{
    m_gridWidth = width;
    m_gridHeight = height;
    m_gridScale = scale;

    // 버퍼 초기화 (지오메트리 생성)
    if (InitiBuffers(device) == false)
    {
        return false;
    }

    return true;
} // Init


void GridModelBuffer::Shutdown()
{
    m_vertices.clear();
} // Shutdown


bool GridModelBuffer::InitiBuffers(ID3D11Device* device)
{
    m_vertexCount = m_gridWidth * m_gridHeight;
    m_indexCount = (m_gridWidth - 1) * (m_gridHeight - 1) * 6;

    m_vertices.resize(m_vertexCount);
    std::vector<unsigned long> indices(m_indexCount);

    // 버텍스 생성
    for (unsigned int j = 0; j < m_gridHeight; j++)
    {
        for (unsigned int i = 0; i < m_gridWidth; i++)
        {
            unsigned int index = (m_gridHeight * j) + i;

            float x = (float)i * m_gridScale;
            float z = (float)j * m_gridScale;

            m_vertices[index].position = XMFLOAT3(x, 0.0f, z);
            m_vertices[index].texture = XMFLOAT2((float)i / (m_gridWidth - 1), (float)j / (m_gridHeight - 1));
            m_vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        }
    }

    //  인덱스 생성
    unsigned int index = 0;
    for (unsigned int j = 0; j < (m_gridHeight - 1); j++)
    {
        for (unsigned int i = 0; i < (m_gridWidth - 1); i++)
        {
            int bottomLeft = (m_gridHeight * j) + i;
            int bottomRight = (m_gridHeight * j) + (i + 1);
            int topLeft = (m_gridHeight * (j + 1)) + i;
            int topRight = (m_gridHeight * (j + 1)) + (i + 1);

            // 삼각형 1 (Left-Bottom Quad)
            indices[index++] = topLeft;
            indices[index++] = topRight;
            indices[index++] = bottomLeft;

            // 삼각형 2 (Right-Top Quad)
            indices[index++] = bottomLeft;
            indices[index++] = topRight;
            indices[index++] = bottomRight;
        }
    }

    // 버텍스 버퍼 생성
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = m_vertices.data();

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.GetAddressOf())))
        return false;

    // 인덱스 버퍼 생성
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.GetAddressOf())))
        return false;

    return true;
} // InitiBuffers


void GridModelBuffer::Render(ID3D11DeviceContext* context)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
} // Render


int GridModelBuffer::GetVertexCount()
{
    return m_vertexCount;
} // GetVertexCount


void GridModelBuffer::CopyVertexArray(void* nodeVertexArray)
{
    memcpy(nodeVertexArray, m_vertices.data(), sizeof(VertexType) * m_vertexCount);
} // CopyVertexArray