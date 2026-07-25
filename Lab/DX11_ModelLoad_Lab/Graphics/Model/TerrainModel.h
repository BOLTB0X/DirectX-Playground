// Graphics/Model/TerrainModel.h
#pragma once
#include <d3d11.h>
#include <vector>
#include <memory>
// Graphics
#include "HeightMap/HeightMap.h"
#include "Model/Mesh/VertexTypes.h"
#include "Model/Texture/Material.h"
#include "Shader/ConstantBuffer.h"

class Frustum;
class TerrainModelCell;
class HeightMap;
class TerrainShader;
class CloudShader;

class TerrainModel {
public:
    TerrainModel();
    TerrainModel(const TerrainModel&) = delete;
    ~TerrainModel();

    bool InitConstantBuffer(ID3D11Device*);
    void InitHeightMap();
    void InitMaterial(const Material&);
    
    void Render(ID3D11DeviceContext*, TerrainShader*, Frustum*, DirectX::XMMATRIX);

public:
    bool CreateCells(ID3D11Device*, int cellDimension = 42);
    void AddMaterial(const Material&);

public:
    const Material& GetMaterial(size_t) const;
    const std::vector<std::unique_ptr<TerrainModelCell>>& GetCell() const { return m_cells; }
    const HeightMap* GetHeightMap() { return m_HeightMap.get(); }
    const int GetWidth() const { return m_terrainWidth; }
    const int GetHeight() const { return m_terrainHeight; }

public:
    void SetHeightMap(std::unique_ptr<HeightMap>);
    void SetTime(float time) { m_time = time; }

private:
    std::vector<std::unique_ptr<TerrainModelCell>> m_cells;
    std::vector<Material> m_materials;
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
    std::unique_ptr<HeightMap> m_HeightMap;

    int m_terrainWidth, m_terrainHeight;
    float m_time;
}; // TerrainModel