#include "Pch.h"
#include "QuadTree.h"
#include "GridModelBuffer.h"
#include "Frustum.h"
#include "Shader/WaterShader.h"
// Common
#include "DebugHelper.h"
#include "MathHelper.h"


QuadTree::QuadTree()
	: m_triangleCount(0),
	m_drawCount(0)
{
	m_parentNode = std::make_unique<NodeType>();
} // QuadTree


QuadTree::~QuadTree()
{
	Shutdown();
} // ~QuadTree


bool QuadTree::Init(GridModelBuffer* modelBuffer, ID3D11Device* device)
{
    int vertexCount = modelBuffer->GetVertexCount();
    m_triangleCount = vertexCount / 3;

    // 버텍스 리스트 임시 복사
    m_vertexList.resize(vertexCount);
    modelBuffer->CopyVertexArray(m_vertexList.data());

    float centerX, centerZ, width;
    CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

    // 부모 노드 생성 및 재귀적 트리 구축
    m_parentNode = std::make_unique<NodeType>();
    CreateTreeNode(m_parentNode.get(), centerX, centerZ, width, device);

    // 트리 구축 후 임시 리스트 해제
    m_vertexList.clear();
    m_vertexList.shrink_to_fit();

    return true;
} // Init


void QuadTree::Shutdown()
{
    m_vertexList.clear();
    m_parentNode.reset();
} // Shutdown


void QuadTree::Render(Frustum* frustum, ID3D11DeviceContext* deviceContext, WaterShader* shader)
{
    m_drawCount = 0;
    if (frustum == nullptr || deviceContext == nullptr)
    {
        DebugHelper::DebugPrint("QuadTree::Render: deviceContext || frustum  이 nullptr");
        return;
	}

    if (m_parentNode)
    {
        RenderNode(m_parentNode.get(), frustum, deviceContext, shader);
    }
} // Render


int QuadTree::GetDrawCount() const
{
    return m_drawCount;
} // GetDrawCount


void QuadTree::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
    node->positionX = positionX;
    node->positionZ = positionZ;
    node->width = width;
    node->triangleCount = 0;

    node->nodes[0] = 0;
    node->nodes[1] = 0;
    node->nodes[2] = 0;
    node->nodes[3] = 0;

    // 이 노드 안에 있는 삼각형의 개수를 세어 봄
    int numTriangles = CountTriangles(positionX, positionZ, width);

	// case 1: 삼각형이 하나도 없는 경우, 이 노드는 필요 없으므로 그냥 리턴
    if (numTriangles == 0) return;

	// case 2: 삼각형이 너무 많아서 더 이상 분할이 불가능한 경우, 이 노드는 리프 노드가 됨
    // 분할 조건: 최대 삼각형 개수 초과 시
    float offsetX, offsetZ;
	int count;
    if (numTriangles > MAX_TRIANGLES_PER_NODE)
    {
        float childWidth = width / 2.0f;
        float offset = width / 4.0f;

        // 4개의 자식 노드 좌표 설정 (NW, NE, SW, SE)
        float offsetsX[4] = { -offset, offset, -offset, offset };
        float offsetsZ[4] = { offset, offset, -offset, -offset };

        for (unsigned int i = 0; i < 4; i++)
        {
            // 새로운 자식 노드의 위치 오프셋을 계산
            offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
            offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

            // 새 노드에 삼각형이 있는지 확인
            count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
            if (count > 0)
            {
                // 새 노드가 위치할 영역 안에 삼각형이 있다면 자식 노드를 생성
                node->nodes[i] = std::make_unique<NodeType>();

                // 이제 이 새로운 자식 노드부터 트리를 확장
                CreateTreeNode(node->nodes[i].get(), (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
            }
        }
        return;
    }

	// case 3: 이 노드가 비어 있지 않고 삼각형 개수가 최대값보다 작으면
    // 삼각형이 적당히 있어서 더 이상 분할이 필요 없는 경우, 이 노드는 리프 노드가 됨
    // --- 리프 노드(최하단): 실제 버퍼 생성 ---
    node->triangleCount = numTriangles;
    int vertexCount = numTriangles * 3;

    std::vector<VertexType> vertices(vertexCount);
    std::vector<unsigned long> indices(vertexCount);

    int vertexIndex = 0;
    for (unsigned int i = 0; i < m_triangleCount; i++)
    {
        if (IsTriangleContained(i, positionX, positionZ, width))
        {
            for (unsigned int v = 0; v < 3; v++)
            {
                vertices[vertexIndex] = m_vertexList[i * 3 + v];
                indices[vertexIndex] = vertexIndex;
                vertexIndex++;
            } // for - v
        }
    } // for

    // 버텍스 버퍼 생성
    D3D11_BUFFER_DESC vDesc = { sizeof(VertexType) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
    D3D11_SUBRESOURCE_DATA vData = { vertices.data(), 0, 0 };
    device->CreateBuffer(&vDesc, &vData, node->vertexBuffer.GetAddressOf());

    // 인덱스 버퍼 생성
    D3D11_BUFFER_DESC iDesc = { sizeof(unsigned long) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
    D3D11_SUBRESOURCE_DATA iData = { indices.data(), 0, 0 };
    device->CreateBuffer(&iDesc, &iData, node->indexBuffer.GetAddressOf());
} // CreateTreeNode


int QuadTree::CountTriangles(float positionX, float positionZ, float width)
{
    int count = 0;
    for (unsigned int i = 0; i < m_triangleCount; i++)
    {
        if (IsTriangleContained(i, positionX, positionZ, width))
        {
            count++;
        }
    }

    return count;
} // CountTriangles


bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
    float radius = width / 2.0f;
    int vertexIndex = index * 3;

    // 삼각형의 세 정점 좌표 가져오기
    float x[3], z[3];
    for (unsigned int i = 0; i < 3; i++)
    {
        x[i] = m_vertexList[vertexIndex + i].position.x;
        z[i] = m_vertexList[vertexIndex + i].position.z;
    }

    // 노드 경계 계산
    float nodeMinX = positionX - radius;
    float nodeMaxX = positionX + radius;
    float nodeMinZ = positionZ - radius;
    float nodeMaxZ = positionZ + radius;

    // 삼각형의 AABB와 노드 영역 겹침 검사
    float triMinX = min(x[0], min(x[1], x[2]));
    float triMaxX = max(x[0], max(x[1], x[2]));
    float triMinZ = min(z[0], min(z[1], z[2]));
    float triMaxZ = max(z[0], max(z[1], z[2]));

    if (triMinX > nodeMaxX || triMaxX < nodeMinX) return false;
    if (triMinZ > nodeMaxZ || triMaxZ < nodeMinZ) return false;

    return true;
} // IsTriangleContained


void QuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
    float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;

    centerX = 0.0f;
    centerZ = 0.0f;

    // 메쉬의 모든 버텍스의 값을 합산
    for (unsigned int i = 0; i < vertexCount; i++)
    {
        centerX += m_vertexList[i].position.x;
        centerZ += m_vertexList[i].position.z;
    }

    // 그런 다음 이를 버텍스의 개수로 나누어 메쉬의 중간점을 찾음
    centerX = centerX / (float)vertexCount;
    centerZ = centerZ / (float)vertexCount;

    maxWidth = 0.0f;
    maxDepth = 0.0f;

    minWidth = fabsf(m_vertexList[0].position.x - centerX);
    minDepth = fabsf(m_vertexList[0].position.z - centerZ);

    // 모든 버텍스를 순회하면서 메쉬의 최대 및 최소 너비와 깊이를 찾음
    for (unsigned int i = 0; i < vertexCount; i++)
    {
        width = fabsf(m_vertexList[i].position.x - centerX);
        depth = fabsf(m_vertexList[i].position.z - centerZ);

        width = MathHelper::Max(width, maxWidth);
		depth = MathHelper::Max(depth, maxDepth);

		width = MathHelper::Min(width, minWidth);
		depth = MathHelper::Min(depth, minDepth);

    }

    maxX = (float)max(fabs(minWidth), fabs(maxWidth));
    maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

    // 메쉬의 최대 직경을 계산
    meshWidth = max(maxX, maxZ) * 2.0f;
} // CalculateMeshDimensions


void QuadTree::RenderNode(NodeType* node, Frustum* frustum, ID3D11DeviceContext* deviceContext, WaterShader* shader)
{
    // 노드가 보이는지 확인
    // 쿼드 트리에서는 높이가 중요하지 않음
    if (frustum->CheckCube(node->positionX, 0.0f, node->positionZ, node->width / 2.0f)
        == false) return;
    

    // 자식 노드가 있는지 확인
    unsigned int childCount = 0;
    for (unsigned int i = 0; i < 4; i++)
    {
        if (node->nodes[i])
        {
            childCount++;
            RenderNode(node->nodes[i].get(), frustum, deviceContext, shader);
        }
    }

    // 자식 노드가 있다면 부모 노드의 렌더링은 스킵
    if (childCount > 0) return;

    // 리프 노드(최하단)라면 실제 그리기 수행
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, node->vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(node->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    shader->SetShaders(deviceContext);
    int indexCount = node->triangleCount * 3;
    deviceContext->DrawIndexed(indexCount, 0, 0);

    m_drawCount += node->triangleCount;
} // RenderNode


