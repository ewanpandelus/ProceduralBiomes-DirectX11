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

	};
public:
	void Initialise(ID3D11Device* device, int positionalStep);
	std::vector<TerrainType>* GetTerrainMap();


private:
	std::vector<TerrainType> m_terrainMap;
	SimpleMath::Vector2 currentCentrePosition;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_climateMapTextures;
	map<SimpleMath::Vector2, ClimateMap::ClimateMapType*> climateMaps;

};



