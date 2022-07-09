#pragma once
#include "PerlinNoise.h"

using namespace DirectX;

class Terrain
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
	Terrain();
	~Terrain();
	
	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);
	bool Update();

	float* GetPersistance();
	int* GetOctaves();
	float* GetLacunarity();
	float* GetOffset();
	float* GetFrequency();
	float* GetAmplitude();
	HeightMapType* GetHeightMap();

private:
	bool CalculateNormals();
	void Shutdown();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	float Redistribution(float nx, float ny, float exp);
	

private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_lacunarity, m_persistance, m_offset;
	int m_octaves = 0;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
	PerlinNoise perlinNoise;


};

