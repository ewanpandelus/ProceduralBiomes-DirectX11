#include "pch.h"
#include "TerrainLoader.h"

void TerrainLoader::GenerateSurroundingTerrain(ID3D11Device* device,  SimpleMath::Vector2 playerPos, bool initialLoad)
{
	SimpleMath::Vector2 currentPosition;
	if (initialLoad) {
		SetupBounds(playerPos);
	}


	ClimateMap climateMap;

	climateMap.Initialize(64,64);
	int counter = 0;
	int currentScale = 1;
	climateMaps = climateMap.GenerateClimateMaps(m_positionalStep, SimpleMath::Vector2(currentMinXBounds, currentMinZBounds));
	for (int height = 0; height < 3;height++) {
		for (int width = 0; width < 3; width++) {
			currentPosition.x = currentMinXBounds + (width * m_positionalStep);
			currentPosition.y = currentMinZBounds + (height * m_positionalStep);
			TerrainType terrainType;
			Terrain terrain;
		
			terrain.Initialize(device, (m_positionalStep +1)/currentScale, (m_positionalStep + 1) / currentScale, currentScale);
			*terrain.GetAmplitude() = 2;
			*terrain.GetFrequency() = 0.2;
			terrainType.climateMapTex = climateMap.GenerateClimateMapTexture(device, climateMaps[currentPosition]);
			terrain.SetClimateMap(climateMaps[currentPosition]);
			terrain.GenerateHeightMap(device, 1, currentPosition.x, currentPosition.y);
			terrainType.terrain = terrain;
			terrainType.position = currentPosition;
			SetupTerrainBounds(terrainType);
			m_terrainMap.push_back(terrainType);
			counter++;
		}
	}
	RemoveDistantTerrain(playerPos);
}

std::vector<TerrainLoader::TerrainType>* TerrainLoader::GetTerrainMap()
{
	return &m_terrainMap;
}

bool TerrainLoader::OutOfCurrentBounds(SimpleMath::Vector2 playerPos)
{
	
	if (playerPos.x < currentMinXBounds) 
	{
		currentMaxXBounds = currentMinXBounds;
		currentMinXBounds -= m_positionalStep;
		return true;
	}
	else if (playerPos.x > currentMaxXBounds) {
		currentMinXBounds = currentMaxXBounds;
		currentMaxXBounds += m_positionalStep;
		return true;
	}
	if (playerPos.y < currentMinZBounds)
	{
		currentMaxZBounds = currentMinZBounds;
		currentMinZBounds -= m_positionalStep;

		return true;
	}
	else if (playerPos.y > currentMaxZBounds) {
		currentMinZBounds = currentMaxZBounds;
		currentMaxZBounds += m_positionalStep;
		return true;
	}
	return false;
}

void TerrainLoader::RemoveDistantTerrain(SimpleMath::Vector2 playerPos)
{
	//std::map<SimpleMath::Vector2, ClimateMap::ClimateMapType*>::iterator it = climateMaps.begin();
	//for (std::pair<SimpleMath::Vector2, ClimateMap::ClimateMapType*> element : climateMaps) {
	//	
	//	if (SquaredDistance(element.first, playerPos) > m_positionalStep+1 * m_positionalStep+1) {
	//		climateMaps.erase(element.first);
	//	}
	//}
	   //m_world = SimpleMath::Matrix::Identity; //set world back to identity
	//SimpleMath::Matrix positionAccountedFor = SimpleMath::Matrix::CreateTranslation(-m_terrainWidth * m_terrainScale / 2 +64, 0.f, -m_terrainWidth * m_terrainScale / 2+64);
	//m_world = m_world * positionAccountedFor;
	//m_terrainShader.EnableShader(context);
	//m_terrainMap = m_terrainLoader.GetTerrainMap();
	//for each (auto terrain in *m_terrainMap) {
	//    m_world = SimpleMath::Matrix::Identity; //set world back to identity
	//    positionAccountedFor = SimpleMath::Matrix::CreateTranslation(terrain.position.x, 0.f, terrain.position.y);
	//    m_world = m_world * positionAccountedFor;
	//    m_terrainShader.SetBiomeShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,
	//       terrain.climateMapTex.Get(), m_noiseTexture.Get());
	//    terrain.terrain.Render(context);
	//}
}



void TerrainLoader::SetupBounds(SimpleMath::Vector2 pos)
{
	currentMinXBounds = pos.x- m_positionalStep/2;
	currentMaxXBounds = pos.x + m_positionalStep/2;
	
	currentMinZBounds = pos.y - m_positionalStep/2;
	currentMaxZBounds = pos.y + m_positionalStep/2;
}

float TerrainLoader::SquaredDistance(DirectX::SimpleMath::Vector2 v1, DirectX::SimpleMath::Vector2 v2)
{
	return pow((v1.x - v2.x), 2) + pow(v1.y - v2.y,2);
}

void TerrainLoader::SetupTerrainBounds(TerrainType terrain)
{
	terrain.minXBound = terrain.position.x;
	terrain.maxXBound = terrain.position.x + m_positionalStep;
	terrain.minZBound = terrain.position.y;
	terrain.minXBound = terrain.position.y + m_positionalStep;

}

