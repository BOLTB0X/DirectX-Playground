// World/Terrain/HeightMap.cpp
#include "HeightMap.h"
// Common
#include "EngineHelper.h"
// etc
#include <fstream>
#include <iostream>


/* default */
/////////////////////////////////////////////////////

HeightMap::HeightMap()
    : m_width(0),
    m_height(0),
    m_cellSpacing(0.0f),
	m_heightScale(0.0f)
{
    m_minBounds = { 0, 0, 0 };
    m_maxBounds = { 0, 0, 0 };
} // HeightMap


HeightMap::~HeightMap()
{
	Shutdown();
} // ~HeightMap


//bool HeightMap::Init(int width, int height, float scale, const std::string& filename)
//{
//	m_width = width;
//	m_height = height;
//	m_heightScale = scale;
//
//	std::vector<unsigned short> rawImage;
//	if (LoadRawFile(filename, rawImage) == false)
//	{
//		EngineHelper::DebugPrint("HeightMap::Init : 파일 로드 실패 - " + filename);
//		return false;
//	}
//
//	m_HeightMap.clear();
//	m_HeightMap.resize(rawImage.size());
//	ResetBounds();
//
//	for (int j = 0; j < m_height; j++)
//	{
//		for (int i = 0; i < m_width; i++)
//		{
//			int index = (m_width * j) + i;
//
//			m_HeightMap[index].x = (float)i;
//			m_HeightMap[index].y = 0.0f;
//			//m_HeightMap[index].y = (float)rawImage[index] / m_heightScale;
//			m_HeightMap[index].z = (float)j;
//
//			UpdateBounds(m_HeightMap[index].x, m_HeightMap[index].y, m_HeightMap[index].z);
//		}
//	}
//
//	CalculateNormals();
//	return true;
//} // Init


bool HeightMap::Init(const std::vector<ModelVertex>& vertices, const std::vector<unsigned int>& indices)
{
	if (vertices.empty() || indices.empty()) return false;

	m_vertices = vertices;
	m_indices = indices;
	ResetBounds();

	for (const auto& v : m_vertices)
	{
		UpdateBounds(v.position.x, v.position.y, v.position.z);
	}

	return true;
} // Init


void HeightMap::InitPlane(int width, int height)
{
	m_width = width;
	m_height = height;
	m_heightScale = 1.0f;

	m_HeightMap.clear();
	m_HeightMap.resize(m_width * m_height);
	ResetBounds();

	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			int index = (m_width * j) + i;

			m_HeightMap[index].x = (float)i;
			m_HeightMap[index].y = 0.0f;
			m_HeightMap[index].z = (float)j;

			m_HeightMap[index].nx = 0.0f;
			m_HeightMap[index].ny = 1.0f;
			m_HeightMap[index].nz = 0.0f;

			UpdateBounds(m_HeightMap[index].x, m_HeightMap[index].y, m_HeightMap[index].z);
		}
	}

	return;
} // InitPlane


void HeightMap::Shutdown()
{
	m_HeightMap.clear();
	return;
} // Shutdown

////////////////////////////////////////////////////////////////////////////////////////////////////

/* public: get */
////////////////////////////////////////////////////////////

void HeightMap::GetBounds(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max) const
{
    min = m_minBounds;
    max = m_maxBounds;
} // GetBounds


//float HeightMap::GetHeightAtGrid(float x, float z) const
//{
//	if (x < m_minBounds.x || x >= m_maxBounds.x - 1.0f || z < m_minBounds.z || z >= m_maxBounds.z - 1.0f)
//		return m_minBounds.y;
//
//	// 현재 좌표가 속한 그리드 셀의 인덱스 찾기
//	int gridX = static_cast<int>(floor(x));
//	int gridZ = static_cast<int>(floor(z));
//
//	// 셀의 네 모서리 높이값 추출
//	float h00 = m_HeightMap[(gridZ * m_width) + gridX].y; // 좌하
//	float h10 = m_HeightMap[(gridZ * m_width) + (gridX + 1)].y; // 우하
//	float h01 = m_HeightMap[((gridZ + 1) * m_width) + gridX].y; // 좌상
//	float h11 = m_HeightMap[((gridZ + 1) * m_width) + (gridX + 1)].y; // 우상
//
//	//  셀 내부에서의 (0.0 ~ 1.0)
//	float dx = x - static_cast<float>(gridX);
//	float dz = z - static_cast<float>(gridZ);
//
//	// 이중 선형 보간
//	float topHeight = h00 + (h10 - h00) * dx;
//	float bottomHeight = h01 + (h11 - h01) * dx;
//
//	return topHeight + (bottomHeight - topHeight) * dz;
//} // GetHeightAtGrid


float HeightMap::GetHeightAtMesh(float x, float z) const
{
	if (x < m_minBounds.x || x > m_maxBounds.x || z < m_minBounds.z || z > m_maxBounds.z)
		return m_minBounds.y;

	float height = 0.0f;

	for (size_t i = 0; i < m_indices.size(); i += 3)
	{
		float v0[3] = { m_vertices[m_indices[i]].position.x, m_vertices[m_indices[i]].position.y, m_vertices[m_indices[i]].position.z };
		float v1[3] = { m_vertices[m_indices[i + 1]].position.x, m_vertices[m_indices[i + 1]].position.y, m_vertices[m_indices[i + 1]].position.z };
		float v2[3] = { m_vertices[m_indices[i + 2]].position.x, m_vertices[m_indices[i + 2]].position.y, m_vertices[m_indices[i + 2]].position.z };

		if (const_cast<HeightMap*>(this)->CheckHeightOfTriangle(x, z, height, v0, v1, v2))
		{
			return height;
		}
	}

	return m_minBounds.y;
} // GetHeightAtMesh
////////////////////////////////////////////////////////////

/* public: check */
////////////////////////////////////////////////////////////

bool HeightMap::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3];
	float Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3];
	float magnitude, D, denominator, numerator, t, determinant;

	// 투사되는 광선의 시작 위치
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// 광선이 발사되는 방향
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// 주어진 세 점에서 두 변의 길이를 계산
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// 두 변으로부터 삼각형의 법선을 계산
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// 원점에서 평면까지의 거리
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// 방정식의 분모
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// 0으로 나누는 오류를 방지하기 위해 결과값이 0에 너무 가까워지지 않도록 주의
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// 방정식의 분자
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// 두 선분이 삼각형과 만나는 점을 계산
	t = numerator / denominator;

	// 작교벡터
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// 삼각형의 꼭짓점
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// 첫 번째 모서리의 법선을 계산
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 바깥에 있는지 확인
	if (determinant > 0.001f)
	{
		return false;
	}

	// 두 번째 모서리의 법선을 계산
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 바깥에 있는지 확인
	if (determinant > 0.001f)
	{
		return false;
	}

	// 세 번째 모서리의 법선을 계산
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// 행렬식을 계산하여 내부, 외부 또는 모서리에 있는지 확인
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 세 번째 모서리의 법선을 계산
	if (determinant > 0.001f)
	{
		return false;
	}

	height = Q[1];

	return true;
} // CheckHeightOfTriangle

////////////////////////////////////////////////////////////

/* private */
////////////////////////////////////////////////////////////


bool HeightMap::LoadRawFile(const std::string& filename, std::vector<unsigned short>& outRawData)
{
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		EngineHelper::DebugPrint("HeightMap::Init : LoadRawFile 에러");
		return false;
	}

	size_t imageSize = (size_t)m_width * m_height;
	outRawData.resize(imageSize);

	size_t count = fread(outRawData.data(), sizeof(unsigned short), imageSize, filePtr);
	fclose(filePtr);

	return (count == imageSize);
} // LoadRawFile


void HeightMap::ResetBounds()
{
	m_minBounds = { FLT_MAX, FLT_MAX, FLT_MAX };
	m_maxBounds = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
} // ResetBounds


void HeightMap::UpdateBounds(float x, float y, float z)
{
	m_minBounds.x = (std::min)(m_minBounds.x, x);
	m_minBounds.y = (std::min)(m_minBounds.y, y);
	m_minBounds.z = (std::min)(m_minBounds.z, z);

	m_maxBounds.x = (std::max)(m_maxBounds.x, x);
	m_maxBounds.y = (std::max)(m_maxBounds.y, y);
	m_maxBounds.z = (std::max)(m_maxBounds.z, z);
} // UpdateBounds


void HeightMap::CalculateNormals()
{
	// 모든 버테글 순회하며 노말 계산
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			// 인접한 점들의 위치 가져오기 (경계 처리 포함)
			// 좌, 우 점의 높이 차이로 X축 기울기 계산
			int left = (std::max)(i - 1, 0);
			int right = (std::min)(i + 1, m_width - 1);
			float hL = m_HeightMap[(j * m_width) + left].y;
			float hR = m_HeightMap[(j * m_width) + right].y;

			// 상, 하 점의 높이 차이로 Z축 기울기 계산
			int up = (std::max)(j - 1, 0);
			int down = (std::min)(j + 1, m_height - 1);
			float hU = m_HeightMap[(up * m_width) + i].y;
			float hD = m_HeightMap[(down * m_width) + i].y;

			// 중앙 차분법을 이용한 법선 벡터 계산
			// 수평 벡터: (2, hR - hL, 0)
			// 수직 벡터: (0, hD - hU, 2)
			DirectX::XMFLOAT3 normal;
			float nx = hL - hR;
			float ny = 2.0f;
			float nz = hU - hD;

			DirectX::XMVECTOR vNormal = DirectX::XMVectorSet(nx, ny, nz, 0.0f);
			vNormal = DirectX::XMVector3Normalize(vNormal);

			int index = (j * m_width) + i;
			DirectX::XMStoreFloat3(&normal, vNormal);

			m_HeightMap[index].nx = normal.x;
			m_HeightMap[index].ny = normal.y;
			m_HeightMap[index].nz = normal.z;
		}
	}
} // CalculateNormals