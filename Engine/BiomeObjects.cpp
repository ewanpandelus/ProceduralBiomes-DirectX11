#include "pch.h"
#include "BiomeObjects.h"



void BiomeObjects::SetLargeModels(ModelClass desertModel, ModelClass forestModel, ModelClass snowModel)
{
	this->desertModel = desertModel;
	this->forestModel = forestModel;
	this->snowModel = snowModel;
}

void BiomeObjects::SetLargeModelsTextures(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> forestTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> snowTexture)
{
	this->desertTexture = desertTexture;
	this->forestTexture = forestTexture;
	this->snowTexture = snowTexture;
}


std::vector<BiomeObjects::BiomeObjectType> BiomeObjects::SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions, int terrainWidth)
{
	std::vector<BiomeObjects::BiomeObjectType> m_objectMap;
	int index = 0;
	for each (auto pos in gridPositions)
	{
		SimpleMath::Vector2 positionOnClimateMap =SimpleMath::Vector2((int)pos.x, (int)pos.y);

		index = (terrainWidth * positionOnClimateMap.y) + positionOnClimateMap.x;  
		SimpleMath::Vector3 position = SimpleMath::Vector3(pos.x, m_heightMap[index].y, pos.y);
		BiomeObjects::BiomeObjectType biomeObj = AssignModelBasedOnClimate(position, m_climateMap[index].climateClassification);
		//m_objectMap.push_back(AssignModelBasedOnClimate(position, m_climateMap[index].climateClassification));
		m_entityData->IncreaseEntityCount(biomeObj.modelID, biomeObj.position);
	}
	return m_objectMap;
}
void BiomeObjects::AddToObjects(int modelID, int biomeType)
{
	switch (biomeType) {
	case 0:
		m_desertObjects.push_back(modelID);
		break;
	case 1:
		m_forestObjects.push_back(modelID);
		break;
	case 2:
		m_snowObjects.push_back(modelID);
		break;
	default:
		return;
	}
}



int BiomeObjects::GetRandomObjectFromBiome(int biomeType)
{
	int listSize = 0;
	switch (biomeType) {
	case 0:
		listSize = m_desertObjects.size();
		return m_desertObjects[rand() % listSize];
		break;
	case 1:
		listSize = m_forestObjects.size();
		return m_forestObjects[rand() % listSize];
		break;
	case 2:
		listSize = m_snowObjects.size();
		return m_snowObjects[rand() % listSize];
		break;
	default:
		return -1;
	}
}

BiomeObjects::BiomeObjectType BiomeObjects::AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector3 climateClassification)
{

	int percentage = rand() % 101;
	float desertPercent = climateClassification.x * 100;
	float forestPercent = climateClassification.y * 100;
	float snowPercent = climateClassification.z * 100;
	if (desertPercent > 55 || percentage < desertPercent) {
		return SetupObject(GetRandomObjectFromBiome(0), position);
	}
	if (forestPercent > 55 || (percentage < desertPercent + forestPercent)) {
		return SetupObject(GetRandomObjectFromBiome(1), position);
	}
	if (snowPercent > 55 || percentage <= (desertPercent + forestPercent + snowPercent+0.01f)) {
		return SetupObject(GetRandomObjectFromBiome(2), position);

	}
}





	//else  return PickFirstChoice(percentage, forestPercent) ? SetupObject(forestModel, forestTexture, x , z, 1) : SetupObject(desertModel, desertTexture, x, z, 2);


	//https://nbertoa.wordpress.com/2016/02/02/instancing-vs-geometry-shader-vs-vertex-shader/


BiomeObjects::BiomeObjectType BiomeObjects::SetupObject(int modelID,  SimpleMath::Vector3 position)
{
	BiomeObjectType biomeObject;
	biomeObject.modelID = modelID;
	biomeObject.position = position;
	return biomeObject;
}
void BiomeObjects::SetCellWidth(float cellSize)
{
	this->currentCellWidth = cellSize;
}

void BiomeObjects::SetClimateMap(ClimateMap::ClimateMapType* m_climateMap)
{
	this->m_climateMap = m_climateMap;
}

void BiomeObjects::SetHeightMap(Terrain::HeightMapType* m_heightMap)
{
	this->m_heightMap = m_heightMap;
}

void BiomeObjects::SetEntityData(EntityData* entityData)
{
	m_entityData = entityData;
}
