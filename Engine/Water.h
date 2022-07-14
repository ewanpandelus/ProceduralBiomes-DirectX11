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
		float nx, ny, nz;
		float u, v;
	};
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

public:
	Water();
	~Water();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);
	bool Update();
	float pressed = -0.1f;
	float* GetPersistance();
	int* GetOctaves();
	float* GetLacunarity();
	float* GetOffset();
	float* GetFrequency();
	float* GetAmplitude();
	HeightMapType* GetHeightMap();

	void SetTerrainHeightMap(Terrain terrain);

private:
	bool CalculateNormals();
	void Shutdown();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	float Redistribution(float nx, float ny, float exp);
	Terrain::HeightMapType* m_terrainHeightMap;
private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_lacunarity, m_persistance, m_offset;
	int m_octaves = 0;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
	PerlinNoise perlinNoise;


};


