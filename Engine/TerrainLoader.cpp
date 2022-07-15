#include "pch.h"
#include "TerrainLoader.h"

void TerrainLoader::Initialise(ID3D11Device* device, int positionalStep)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> climateMapTex;
	SimpleMath::Vector2 currentPosition = SimpleMath::Vector2(0, 0);
	for (int height = 0; height < 3;height++) {
		for (int width = 0; width < 3; width++) {
			TerrainLoader::TerrainType terrainType;
			currentPosition.x = (width * (positionalStep)) - (positionalStep);
			currentPosition.y = (height * (positionalStep)) - (positionalStep);
			ClimateMap climateMap;
			Terrain terrain;
		/*	if (width && height == 1) {*/
				climateMap.Initialize(64, 64);
				terrainType.climateMap = climateMap.GenerateClimateMap();
				//terrain.Initialize(device, 64, 64);
				//climateMapTex = climateMap.GenerateClimateMapTexture(device, 64);
			//}
			/*else {
				climateMap.Initialize(32, 32);
				terrain.Initialize(device, 32, 32);
				terrainType.climateMap = climateMap.GenerateClimateMap(currentPosition.x, currentPosition.y);
				climateMapTex = climateMap.GenerateClimateMapTexture(device, 32);
			}*/

			//terrain.GenerateHeightMap(device);
			terrainType.terrain = terrain;
			terrainType.climateMapTex = climateMapTex;
			terrainType.position = currentPosition;

			m_terrainMap.push_back(terrainType);
		}
	}
}

std::vector<TerrainLoader::TerrainType>* TerrainLoader::GetTerrainMap()
{
	return &m_terrainMap;
}

