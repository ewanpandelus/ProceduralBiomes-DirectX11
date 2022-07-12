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
	return SimpleMath::Vector2((int)(x), (int)(y));
}

std::vector<BiomeObjects::BiomeObjectType> BiomeObjects::SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions)
{
	std::vector<BiomeObjects::BiomeObjectType> m_objectMap;
	int index = 0;
	for each (auto pos in gridPositions)
	{
		SimpleMath::Vector2 positionOnClimateMap = ConvertPointToTerrainPosition(pos.x, pos.y);

		index = (128 * positionOnClimateMap.y) + positionOnClimateMap.x;  //128 is width
		SimpleMath::Vector3 position = SimpleMath::Vector3(pos.x, m_heightMap[index].y, pos.y);
		m_objectMap.push_back(AssignModelBasedOnClimate(position, m_climateMap[index].climateClassification));
	}
	return m_objectMap;
}
void BiomeObjects::AddToObjects(ModelClass model, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, int biomeType)
{
	ObjectType obj;
	obj.model = model;
	obj.texture = texture;
	switch (biomeType) {
	case 0:
		m_desertObjects.push_back(obj);
		break;
	case 1:
		m_forestObjects.push_back(obj);
		break;
	case 2:
		m_snowObjects.push_back(obj);
		break;
	default:
		return;
	}
}



BiomeObjects::ObjectType BiomeObjects::GetRandomObjectFromBiome(int biomeType)
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
		BiomeObjects::ObjectType obj;
		return obj;
	}
}

BiomeObjects::BiomeObjectType BiomeObjects::AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector3 climateClassification)
{

	int percentage = rand() % 101;
	float desertPercent = climateClassification.x * 100;
	float forestPercent = climateClassification.y * 100;
	float snowPercent = climateClassification.z * 100;
	if (desertPercent > 60 || percentage < desertPercent) {
		BiomeObjects::ObjectType obj = GetRandomObjectFromBiome(0);
		return SetupObject(obj.model, obj.texture, position);
	}
	if (forestPercent > 60 || (percentage < desertPercent + forestPercent)) {
		BiomeObjects::ObjectType obj = GetRandomObjectFromBiome(1);
		return SetupObject(obj.model, obj.texture, position);
	}
	if (snowPercent > 60 || percentage <= (desertPercent + forestPercent + snowPercent+0.01f)) {
		BiomeObjects::ObjectType obj = GetRandomObjectFromBiome(2);
		return SetupObject(obj.model, obj.texture, position);
	}
}





	//else  return PickFirstChoice(percentage, forestPercent) ? SetupObject(forestModel, forestTexture, x , z, 1) : SetupObject(desertModel, desertTexture, x, z, 2);


	//https://nbertoa.wordpress.com/2016/02/02/instancing-vs-geometry-shader-vs-vertex-shader/


BiomeObjects::BiomeObjectType BiomeObjects::SetupObject(ModelClass associatedModel, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> associatedTexture, SimpleMath::Vector3 position)
{
	BiomeObjectType biomeObject;
	biomeObject.model = associatedModel;
	biomeObject.texture = associatedTexture;
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
