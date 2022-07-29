#include "pch.h"
#include "TerrainLoader.h"

void TerrainLoader::GenerateSurroundingTerrain(ID3D11Device* device, int positionalStep, SimpleMath::Vector2 playerPos)
{

	SimpleMath::Vector2 currentPosition = SimpleMath::Vector2(playerPos.x - (positionalStep +1), playerPos.y - (positionalStep + 1));
	ClimateMap climateMap;

	climateMap.Initialize(64,64);
	int counter = 0;
	int currentScale = 1;
	climateMaps = climateMap.GenerateClimateMaps(positionalStep, playerPos);
	for (int height = 0; height < 3;height++) {
		for (int width = 0; width < 3; width++) {
		
			TerrainType terrainType;
			currentPosition.x = (width * positionalStep) + (playerPos.x - (positionalStep + 1));
			currentPosition.y = (height * positionalStep) + (playerPos.y - (positionalStep + 1));
			Terrain terrain;
			terrain.Initialize(device, (positionalStep+1)/currentScale, (positionalStep + 1) / currentScale, currentScale);
			*terrain.GetAmplitude() = 2;
			*terrain.GetFrequency() = 0.2;
			terrainType.climateMapTex = climateMap.GenerateClimateMapTexture(device, climateMaps[currentPosition]);
			terrain.SetClimateMap(climateMaps[currentPosition]);
			terrain.GenerateHeightMap(device, 1, currentPosition.x, currentPosition.y);
			terrainType.terrain = terrain;
			terrainType.position = currentPosition;
			m_terrainMap.push_back(terrainType);
			counter++;
			
		}
	}

}

std::vector<TerrainLoader::TerrainType>* TerrainLoader::GetTerrainMap()
{
	return &m_terrainMap;
}

