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

SimpleMath::Vector2 BiomeObjects::ConvertPointToTerrainPosition(int x, int y)
{
	return SimpleMath::Vector2((int)(x / 2), (int)(y / 2));
}

std::vector<BiomeObjects::BiomeObjectType> BiomeObjects::SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions)
{
	std::vector<BiomeObjects::BiomeObjectType> m_objectMap;
	int index = 0;
	for each (auto pos in gridPositions)
	{
		SimpleMath::Vector2 positionOnClimateMap = ConvertPointToTerrainPosition(pos.x, pos.y);
		index = (128 * positionOnClimateMap.y) + positionOnClimateMap.x;  //128 is width
		m_objectMap.push_back(AssignModelBasedOnClimate(pos.x,  pos.y, m_climateMap[index].climateClassification));
	}
	return m_objectMap;
}
bool BiomeObjects::PickFirstChoice(float percentage, float firstPercentage)
{
	return percentage <= firstPercentage;
}
//BiomeObjects::BiomeObjectType BiomeObjects::AssignModelBasedOnClimate(float x, float z, SimpleMath::Vector2 climateClassification)
//{
//	
//	int percentage = rand() % 101;
//	float desertPercent = climateClassification.x * 100;  
//	float forestPercent = climateClassification.y * 100;  
//	float snowPercent = climateClassification.z * 100;    
//
//
//	
//	if (desertPercent == 0) {
//		return PickFirstChoice(percentage, forestPercent) ? SetupObject(forestModel, forestTexture, x , z, 1) : SetupObject(snowModel, snowTexture, x, z, 2);
//	}
//	else if (forestPercent == 0) {
//		return PickFirstChoice(percentage, desertPercent) ? SetupObject(desertModel, desertTexture, x, z, 0) : SetupObject(snowModel, snowTexture, x, z, 2);
//	}
//	else   {
//		return PickFirstChoice(percentage, forestPercent) ? SetupObject(forestModel, forestTexture, x, z, 1) : SetupObject(desertModel, desertTexture, x, z, 0);
//	}
//	
//
//
//}
BiomeObjects::BiomeObjectType BiomeObjects::AssignModelBasedOnClimate(float x, float z, SimpleMath::Vector2 climateClassification)
{
	
	int percentage = rand() % 101;
	float desertPercent = climateClassification.x * 100;  
	float forestPercent = climateClassification.y * 100;  
	
	if (desertPercent > 55) {
		return SetupObject(desertModel, desertTexture, x, z, 2);
	}
	else if (forestPercent > 55) {
		return SetupObject(forestModel, forestTexture, x, z, 1);
	}
	else  return PickFirstChoice(percentage, forestPercent) ? SetupObject(forestModel, forestTexture, x , z, 1) : SetupObject(desertModel, desertTexture, x, z, 2);

	
	//https://nbertoa.wordpress.com/2016/02/02/instancing-vs-geometry-shader-vs-vertex-shader/

}
BiomeObjects::BiomeObjectType BiomeObjects::SetupObject(ModelClass associatedModel, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> associatedTexture, float x, float z, int id)
{
	BiomeObjectType biomeObject;
	biomeObject.model = associatedModel;
	biomeObject.texture = associatedTexture;
	biomeObject.x = x;
	biomeObject.z = z;
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
