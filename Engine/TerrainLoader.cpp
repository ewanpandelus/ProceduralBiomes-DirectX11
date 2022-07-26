#include "pch.h"
#include "TerrainLoader.h"

void TerrainLoader::Initialise(ID3D11Device* device, int positionalStep)
{

	SimpleMath::Vector2 currentPosition = SimpleMath::Vector2(0, 0);
	ClimateMap climateMap;

	climateMap.Initialize(64,64);
	TerrainLoader::TerrainType terrainType[9];
	int counter = 0;

	for (int height = 0; height < 3;height++) {
		for (int width = 0; width < 3; width++) {
		
			currentPosition.x = (width * positionalStep);
			currentPosition.y = (height * positionalStep);

			Terrain terrain;
			climateMap.Initialize(64, 64);
			climateMap.GenerateClimateMap(currentPosition.x, currentPosition.y);
			terrain.Initialize(device, 64, 64, 1);



			*terrain.GetAmplitude() = 3;
			*terrain.GetFrequency() = 0.4;

			terrain.GenerateHeightMap(device, 1, currentPosition.x, currentPosition.y);

			terrainType[counter].terrain = terrain;

			terrainType[counter].position = currentPosition;
			terrainType[counter].climateMapTex = climateMap.GenerateClimateMapTexture(device);
			m_terrainMap.push_back(terrainType[counter]);
			counter++;
			
		}
	}

}

std::vector<TerrainLoader::TerrainType>* TerrainLoader::GetTerrainMap()
{
	return &m_terrainMap;
}

