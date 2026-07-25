// Model/HeightMap/HeightMap.h
#pragma once
#include <vector>
#include <DirectXMath.h>
#include <string>
// Graphics
#include "HeightMapType.h"
#include "Model/Mesh/VertexTypes.h"

class HeightMap {
public:
    HeightMap();
    HeightMap(const HeightMap&) = delete;
    ~HeightMap();

   // bool Init(int width, int height, float scale, const std::string& filename);
    bool Init(const std::vector<ModelVertex>&, const std::vector<unsigned int>&);
    void InitPlane(int, int);

    void Shutdown();

public:
    const std::vector<HeightMapType>& GetRawData() const { return m_HeightMap; }
    void GetBounds(DirectX::XMFLOAT3&, DirectX::XMFLOAT3&) const;
    //float GetHeightAtGrid(float, float) const;
    float GetHeightAtMesh(float, float) const;
    bool CheckHeightOfTriangle(float, float, float&, float v0[3], float v1[3], float v2[3]);

private:
    bool LoadRawFile(const std::string& filename, std::vector<unsigned short>& outRawData);
    void UpdateBounds(float x, float y, float z);
    void ResetBounds();
    void CalculateNormals();

private:
    std::vector<ModelVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<HeightMapType> m_HeightMap;

    DirectX::XMFLOAT3 m_minBounds;
    DirectX::XMFLOAT3 m_maxBounds;

    int m_width, m_height;
    float m_cellSpacing, m_heightScale;;
}; // HeightMap