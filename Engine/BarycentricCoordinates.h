#pragma once
#include "Terrain.h"
class BarycentricCoordinates
{
public:
	float GetYPosition(float x, float z, int heightMapIndex);
	void SetHeightMap(Terrain::HeightMapType* heightMap);
	void SetTerrainScale(float scale);
private:
	Terrain::HeightMapType* m_heightMap;
	float terrainScale = 1;
private:
	void Intersects(DirectX::SimpleMath::Vector2 point, DirectX::SimpleMath::Vector2  v0, DirectX::SimpleMath::Vector2 v1, DirectX::SimpleMath::Vector2 v2,
		float* weight1, float* weight2, float* weight3);
	float Cross(SimpleMath::Vector2 a, SimpleMath::Vector2 b);

};

