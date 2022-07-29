#pragma once
#include "PerlinNoise.h"
#include "ClimateMap.h"
using namespace DirectX;

class Terrain
{
public:
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		SimpleMath::Vector3 triPos1;
		SimpleMath::Vector3 triPos2;
	};
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

public:
	Terrain();
	~Terrain();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight, int scaleIncrease);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*, int scaleIncrease, int xOffset, int zOffset);
	bool Update();

	float* GetPersistance();
	int* GetOctaves();
	float* GetLacunarity();
	float* GetOffset();
	float* GetFrequency();
	float* GetAmplitude();
	float GetMaxDepth();
	HeightMapType* GetHeightMap();
	void SetClimateMap(ClimateMap::ClimateMapType* climateMap);



private:
	bool CalculateNormals();
	void Shutdown();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	float Redistribution(float nx, float ny, float exp);
	void SetupVertex(unsigned long* indices, VertexType* vertices, int* currentIndex, int triangle1Index, int triangle2Index, int triangle3Index);
	float EvaluateNoiseBasedOnClimate(int index, float xOffset, float zOffset, int i, int j);

private:
	bool m_terrainGeneratedToggle;
	float m_maxDepth;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency = 0.3, m_amplitude = 5, m_lacunarity, m_persistance, m_offset;
	int m_octaves = 0;
	HeightMapType* m_heightMap;
	ClimateMap::ClimateMapType* climateMap;
	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
	PerlinNoise perlinNoise;


};

