#pragma once
#include "Terrain.h"
#include "ClimateMap.h"

using namespace DirectX;

class TerrainLoader
{
public:
	struct TerrainType {
		Terrain terrain;
		SimpleMath::Vector2 position;
		ClimateMap::ClimateMapType* climateMap;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> climateMapTex;
		int minXBound;
		int maxXBound;
		int minZBound;
		int maxZBound;
	};
public:
	void GenerateSurroundingTerrain(ID3D11Device* device, SimpleMath::Vector2 playerPos, bool initialLoad);
	std::vector<TerrainType>* GetTerrainMap();
	bool OutOfCurrentBounds(SimpleMath::Vector2 playerPos);
	void RemoveDistantTerrain(SimpleMath::Vector2 playerPos);

private:
	std::vector<TerrainType> m_terrainMap;
	SimpleMath::Vector2 currentCentrePosition;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_climateMapTextures;
	map<SimpleMath::Vector2, ClimateMap::ClimateMapType*> climateMaps;
	int currentMinXBounds = INT16_MAX;
	int currentMaxXBounds = INT16_MIN;
	int currentMinZBounds = INT16_MAX;
	int currentMaxZBounds = INT16_MIN;
	void SetupBounds(SimpleMath::Vector2 pos);
	float SquaredDistance(DirectX::SimpleMath::Vector2 v1, DirectX::SimpleMath::Vector2 v2);
	int m_positionalStep = 63;
	void SetupTerrainBounds(TerrainType terrain);
};



