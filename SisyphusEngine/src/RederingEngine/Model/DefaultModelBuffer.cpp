#include "Pch.h"
#include "DefaultModelBuffer.h"


using namespace DirectX;


DefaultModelBuffer::DefaultModelBuffer()
    : m_vertexCount(4),
    m_indexCount(6)
{}

DefaultModelBuffer::~DefaultModelBuffer() { Shutdown(); }


bool DefaultModelBuffer::Init(ID3D11Device* device, DefaultModelType type)
{
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    switch (type)
    {
    case  DefaultModelType::Quad:
        CreateQuad(vertices, indices);
        break;
    case DefaultModelType::Cube:
        CreateCube(vertices, indices);
        break;
    case DefaultModelType::Sphere:
        CreateSphere(vertices, indices);
        break;
    case DefaultModelType::Ocean:
        CreateOcean(vertices, indices);
		break;
    }

    m_vertexCount = static_cast<int>(vertices.size());
    m_indexCount = static_cast<int>(indices.size());

    return InitBuffers(device, vertices, indices);
} // Init


bool DefaultModelBuffer::Init(ID3D11Device* device,
    const std::vector<VertexType>& vertices,
    const std::vector<unsigned long>& indices)
{
    m_vertexCount = static_cast<int>(vertices.size());
    m_indexCount = static_cast<int>(indices.size());

    return InitBuffers(device, vertices, indices);
} // Init


void DefaultModelBuffer::Shutdown()
{
    m_indexBuffer.Reset();
    m_vertexBuffer.Reset();
} // Shutdown


void DefaultModelBuffer::Render(ID3D11DeviceContext* context)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
} // Render


bool DefaultModelBuffer::InitBuffers(
    ID3D11Device* device,
    std::vector<VertexType> vertices,
    std::vector<unsigned long> indices)
{
    // Vertex Buffer 생성
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinit = { vertices.data() };
    if (FAILED(device->CreateBuffer(&vbd, &vinit, &m_vertexBuffer))) return false;

    // Index Buffer 생성
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long) * m_indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinit = { indices.data() };
    if (FAILED(device->CreateBuffer(&ibd, &iinit, &m_indexBuffer))) return false;

    return true;
} // InitBuffers


void DefaultModelBuffer::CreateQuad(std::vector<VertexType>& vertices, std::vector<unsigned long>& indices, float width, float height)
{
    float w2 = width / 2.0f;
    float h2 = height / 2.0f;

    XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };

    vertices = {
        { {-w2,  h2, 0.0f}, {0.0f, 0.0f}, normal }, // 좌상
        { { w2,  h2, 0.0f}, {1.0f, 0.0f}, normal }, // 우상
        { {-w2, -h2, 0.0f}, {0.0f, 1.0f}, normal }, // 좌하
        { { w2, -h2, 0.0f}, {1.0f, 1.0f}, normal }  // 우하
    };
    indices = { 0, 1, 2, 2, 1, 3 };
} // CreateQuad


void DefaultModelBuffer::CreateCube(std::vector<VertexType>& vertices, std::vector<unsigned long>& indices, float width, float height, float depth)
{
    float w2 = width / 2.0f;
    float h2 = height / 2.0f;
    float d2 = depth / 2.0f;

    XMFLOAT3 p[8] = {
        {-w2,  h2, -d2}, { w2,  h2, -d2}, {-w2, -h2, -d2}, { w2, -h2, -d2}, // Front 4
        {-w2,  h2,  d2}, { w2,  h2,  d2}, {-w2, -h2,  d2}, { w2, -h2,  d2}  // Back 4
    };

    // 각 면의 법선 정의
    XMFLOAT3 nFront = { 0,0,-1 }, nBack = { 0,0,1 }, nTop = { 0,1,0 },
        nBottom = { 0,-1,0 }, nLeft = { -1,0,0 }, nRight = { 1,0,0 };

    vertices = {
        { p[0], {0,0}, nFront }, { p[1], {1,0}, nFront }, { p[2], {0,1}, nFront }, { p[3], {1,1}, nFront },
        { p[5], {0,0}, nBack }, { p[4], {1,0}, nBack }, { p[7], {0,1}, nBack }, { p[6], {1,1}, nBack },
        { p[4], {0,0}, nTop }, { p[5], {1,0}, nTop }, { p[0], {0,1}, nTop }, { p[1], {1,1}, nTop },
        { p[2], {0,0}, nBottom }, { p[3], {1,0}, nBottom }, { p[6], {0,1}, nBottom }, { p[7], {1,1}, nBottom },
        { p[4], {0,0}, nLeft }, { p[0], {1,0}, nLeft }, { p[6], {0,1}, nLeft }, { p[2], {1,1}, nLeft },
        { p[1], {0,0}, nRight }, { p[5], {1,0}, nRight }, { p[3], {0,1}, nRight }, { p[7], {1,1}, nRight }
    };

    for (int i = 0; i < 6; i++)
    {
        int offset = i * 4;
        indices.push_back(offset + 0); indices.push_back(offset + 1); indices.push_back(offset + 2);
        indices.push_back(offset + 2); indices.push_back(offset + 1); indices.push_back(offset + 3);
    }
} // CreateCube


void DefaultModelBuffer::CreateCube(std::vector<VertexType>& vertices, std::vector<unsigned long>& indices,
    float startX, float startY, float startZ,
    float width, float height, float depth)
{
    float maxX = startX + width;
    float maxY = startY + height;
    float maxZ = startZ + depth;

    // 8개의 꼭짓점을 영역에 맞춰 직접 정의
    XMFLOAT3 p[8] = {
        {startX, maxY, startZ}, {maxX,   maxY, startZ}, {startX, startY, startZ}, {maxX,   startY, startZ}, // Front 4
        {startX, maxY, maxZ},   {maxX,   maxY, maxZ},   {startX, startY, maxZ},   {maxX,   startY, maxZ}    // Back 4
    };

    // 노말 정의
    XMFLOAT3 nFront = { 0,0,-1 }, nBack = { 0,0,1 }, nTop = { 0,1,0 },
        nBottom = { 0,-1,0 }, nLeft = { -1,0,0 }, nRight = { 1,0,0 };

    // 버텍스 데이터 (UV는 0~1 사이로 기본 설정)
    vertices = {
        { p[0], {0,0}, nFront }, { p[1], {1,0}, nFront }, { p[2], {0,1}, nFront }, { p[3], {1,1}, nFront },
        { p[5], {0,0}, nBack },  { p[4], {1,0}, nBack },  { p[7], {0,1}, nBack },  { p[6], {1,1}, nBack },
        { p[4], {0,0}, nTop },   { p[5], {1,0}, nTop },   { p[0], {0,1}, nTop },   { p[1], {1,1}, nTop },
        { p[2], {0,0}, nBottom }, { p[3], {1,0}, nBottom }, { p[6], {0,1}, nBottom }, { p[7], {1,1}, nBottom },
        { p[4], {0,0}, nLeft },  { p[0], {1,0}, nLeft },  { p[6], {0,1}, nLeft },  { p[2], {1,1}, nLeft },
        { p[1], {0,0}, nRight }, { p[5], {1,0}, nRight }, { p[3], {0,1}, nRight }, { p[7], {1,1}, nRight }
    };

    // 인덱스는 동일하게 6개 면 처리
    for (int i = 0; i < 6; i++)
    {
        int offset = i * 4;
        indices.push_back(offset + 0); indices.push_back(offset + 1); indices.push_back(offset + 2);
        indices.push_back(offset + 2); indices.push_back(offset + 1); indices.push_back(offset + 3);
    }
} // CreateCube


void DefaultModelBuffer::CreateSphere(std::vector<VertexType>& vertices, std::vector<unsigned long>& indices, float radius, int sliceCount, int stackCount)
{
    // 버텍스 생성
    for (int i = 0; i <= stackCount; ++i)
    {
        float phi = DirectX::XM_PI * (float)i / stackCount;
        for (int j = 0; j <= sliceCount; ++j)
        {
            float theta = DirectX::XM_2PI * (float)j / sliceCount;

            VertexType v;
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            v.texture.x = (float)j / sliceCount;
            v.texture.y = (float)i / stackCount;

            XMVECTOR posVec = XMLoadFloat3(&v.position);
            XMVECTOR normalVec = XMVector3Normalize(posVec);
            XMStoreFloat3(&v.normal, normalVec);

            vertices.push_back(v);
        } // for (int j = 0; j <= sliceCount; ++j)
    } //  for (int i = 0; i <= stackCount; ++i)

    // 인덱스 생성
    for (int i = 0; i < stackCount; ++i)
    {
        for (int j = 0; j < sliceCount; ++j)
        {
            indices.push_back(i * (sliceCount + 1) + j);
            indices.push_back((i + 1) * (sliceCount + 1) + j);
            indices.push_back(i * (sliceCount + 1) + (j + 1));

            indices.push_back(i * (sliceCount + 1) + (j + 1));
            indices.push_back((i + 1) * (sliceCount + 1) + j);
            indices.push_back((i + 1) * (sliceCount + 1) + (j + 1));
        } // for (int j = 0; j < sliceCount; ++j)
    } // for (int i = 0; i < stackCount; ++i)
} // CreateSphere


void DefaultModelBuffer::CreateOcean(std::vector<VertexType>& vertices, std::vector<unsigned long>& indices)
{
    // RasterTek 데이터 (Position, UV, Normal)
    vertices = {
        { {-4.0f, 0.0f,  4.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }, // 좌상
        { { 4.0f, 0.0f, -4.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }, // 우하
        { {-4.0f, 0.0f, -4.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }, // 좌하

        { { 4.0f, 0.0f,  4.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }, // 우상
        { { 4.0f, 0.0f, -4.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }, // 우하
        { {-4.0f, 0.0f,  4.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }  // 좌상
    };

    indices = { 0, 1, 2, 3, 4, 5 };
} // CreateOcean


int DefaultModelBuffer::GetVertexCount() const
{
    return m_vertexCount;
} // GetVertexCount


int DefaultModelBuffer::GetIndexCount() const
{
    return m_indexCount;
} // GetIndexCount