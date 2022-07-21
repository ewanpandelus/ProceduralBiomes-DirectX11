#include "pch.h"
#include "BarycentricCoordinates.h"

float BarycentricCoordinates::GetYPosition(float x, float z, int heightMapIndex)
{
	x *= terrainScale;
	z *= terrainScale;
	SimpleMath::Vector2 v0 = SimpleMath::Vector2(m_heightMap[heightMapIndex].x,  m_heightMap[heightMapIndex].z);
	SimpleMath::Vector2 v1 = SimpleMath::Vector2(m_heightMap[heightMapIndex].triPos1.x, m_heightMap[heightMapIndex].triPos1.z);
	SimpleMath::Vector2 v2 = SimpleMath::Vector2(m_heightMap[heightMapIndex].triPos2.x, m_heightMap[heightMapIndex].triPos2.z);

	SimpleMath::Vector2 point = SimpleMath::Vector2(x, z);

	float weight1, weight2, weight3;
	 Intersects(point , v0, v1, v2, &weight1, &weight2, &weight3);
	 
	 return (m_heightMap[heightMapIndex].y*(weight1))+ (m_heightMap[heightMapIndex].triPos1.y * (weight2)) + (m_heightMap[heightMapIndex].triPos2.y * (weight3));
}

void BarycentricCoordinates::SetHeightMap(Terrain::HeightMapType* heightMap)
{
	m_heightMap = heightMap;
}

void BarycentricCoordinates::SetTerrainScale(float scale)
{
	terrainScale = scale;
}

void BarycentricCoordinates::Intersects(DirectX::SimpleMath::Vector2 point, DirectX::SimpleMath::Vector2  v0, DirectX::SimpleMath::Vector2 v1, DirectX::SimpleMath::Vector2 v2,
	float* weight1, float* weight2, float* weight3){
	double xd = Cross(v0, v1) + Cross(v1, v2) + Cross(v2, v0);

	if (abs(xd) > 1e-13)
	{
		double xa = Cross(v1, v2) + Cross(point, v1 - v2);
		double xb = Cross(v2, v0) + Cross(point, v2 - v0);
		double xc = Cross(v0, v1) + Cross(point, v0 - v1);

		*weight1 = xa / xd;
		*weight2 = xb / xd;
		*weight3 = xc / xd;
	}
}

float BarycentricCoordinates::Cross(SimpleMath::Vector2 a, SimpleMath::Vector2 b)
{
	return a.x* b.y - a.y * b.x;
}
