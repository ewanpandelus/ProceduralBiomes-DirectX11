#include "pch.h"
#include "BiomeObjects.h"






std::vector<BiomeObjects::BiomeObjectType> BiomeObjects::SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions, int terrainWidth, int terrainScale)
{
	std::vector<BiomeObjects::BiomeObjectType> m_objectMap;
	int index = 0;
	for each (auto pos in gridPositions)
	{
		SimpleMath::Vector2 positionOnClimateMap =SimpleMath::Vector2((int)pos.x, (int)pos.y);

		index = (terrainWidth * positionOnClimateMap.y) + positionOnClimateMap.x;  
		SimpleMath::Vector3 position = SimpleMath::Vector3(pos.x, m_barycentricCoords->GetYPosition(pos.x, pos.y,index),  pos.y);
		if (position.y == -10) {
			continue;
		}
		BiomeObjects::BiomeObjectType biomeObj = AssignModelBasedOnClimate(position, m_climateMap[index].climateClassification);
		if (biomeObj.modelID == -1) {
			continue;
		}
		biomeObj.position.x *= terrainScale;
		biomeObj.position.z *= terrainScale;
		m_entityData->IncreaseEntityCount(biomeObj.modelID, biomeObj.position);
	}
	return m_objectMap;
}
void BiomeObjects::AddToObjects(int modelID, int biomeType)
{
	switch (biomeType) {
	case 0:
		if (isSmall) {
			m_desertObjectsSmall.push_back(modelID);
		}
		else {
			m_desertObjectsLarge.push_back(modelID);
		}
		break;
	case 1:
		if (isSmall) {
			m_forestObjectsSmall.push_back(modelID);
		}
		else {
			m_forestObjectsLarge.push_back(modelID);
		}
		break;
	case 2:
		if (isSmall) {
			m_snowObjectsSmall.push_back(modelID);
		}
		else {
			m_snowObjectsLarge.push_back(modelID);
		}
		break;
	case 3:
		if (isSmall) {
			m_waterObjectsSmall.push_back(modelID);
		}
		else {
			m_waterObjectsLarge.push_back(modelID);
		}
	default:
		return;
	}
}

void BiomeObjects::SetIsSmall(bool isSmall)
{
	this->isSmall = isSmall;
}



int BiomeObjects::GetRandomObjectFromBiome(int biomeType)
{
	int listSize = 0;
	switch (biomeType) {
	case 0:
		if (isSmall) {
			listSize = m_desertObjectsSmall.size();
			return -1;//m_desertObjectsSmall[rand() % listSize];
		}
		listSize = m_desertObjectsLarge.size();
		return m_desertObjectsLarge[rand() % listSize];
		break;
	case 1:
		if (isSmall) {
			listSize = m_forestObjectsSmall.size();
			return m_forestObjectsSmall[rand() % listSize];
		}
		listSize = m_forestObjectsLarge.size();
		return m_forestObjectsLarge[rand() % listSize];
		break;
	case 2:
		if (isSmall) {
			//listSize = m_snowObjectsSmall.size();
			return -1;// m_snowObjectsSmall[rand() % listSize];
		}
		listSize = m_snowObjectsLarge.size();
		return m_snowObjectsLarge[rand() % listSize];
		break;
	case 3:
		if (isSmall) {
			return -1;
		}
		listSize = m_waterObjectsLarge.size();
		return m_waterObjectsLarge[rand() % listSize];
		break;
	default:
		return -1;
	}
}

BiomeObjects::BiomeObjectType BiomeObjects::AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector4 climateClassification)
{
	int percentage = rand() % 101;
	int biomeThresholds[4];
	biomeThresholds[0] = 65;
	biomeThresholds[1] = 65;
	biomeThresholds[2] = 53;
	biomeThresholds[3] = 80;


	float biomePercentages[4];
	biomePercentages[0] = climateClassification.x * 100;
	biomePercentages[1] = climateClassification.y * 100;
	biomePercentages[2] = climateClassification.z * 100;
	biomePercentages[3] = climateClassification.w * 100;
	float sumOfBiomePercentages = 0;
	for (int i = 0; i < 4; i++) {
		if (biomePercentages[i] > biomeThresholds[i]) {
			return SetupObject(GetRandomObjectFromBiome(i), position);
		}
	}

	for (int i = 0; i < 4; i++) {
		sumOfBiomePercentages += biomePercentages[i];
		if (i == 3) sumOfBiomePercentages += 0.01f; //Accounts for floating point errors;
		if (percentage < sumOfBiomePercentages && biomePercentages[i] != 0) {
			return SetupObject(GetRandomObjectFromBiome(i), position);
		}

	}
	return SetupObject(GetRandomObjectFromBiome(1), position);

}






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

void BiomeObjects::SetBarycentricCoordinates(BarycentricCoordinates* barycentricCoords)
{
	m_barycentricCoords = barycentricCoords;
}



void BiomeObjects::SetEntityData(EntityData* entityData)
{
	m_entityData = entityData;
}
