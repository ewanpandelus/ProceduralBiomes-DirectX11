#pragma once
#include "PerlinNoise.h"
#include "Terrain.h"

using namespace DirectX;

class Water
{
public:
	struct HeightMapType
	{
		float x, y, z;
		float u, v;
	};
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
	};

public:
	Water();
	~Water();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight, int scale);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);
	bool Update();
	HeightMapType* GetHeightMap();


private:
	void Shutdown();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_scale = 1;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
	PerlinNoise perlinNoise;


};


