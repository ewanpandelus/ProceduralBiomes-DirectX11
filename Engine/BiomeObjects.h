#pragma once
#include "Model.h"
#include "modelclass.h"
#include "ClimateMap.h"
#include "Terrain.h" 
#include "EntityData.h"
class BiomeObjects
{
public:
	
	void SetLargeModels(ModelClass desertModel, ModelClass forestModel, ModelClass snowModel);
	void SetLargeModelsTextures(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> forestTexture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> snowTexture);



	void SetCellWidth(float cellWidth);
	void SetClimateMap(ClimateMap::ClimateMapType* m_climateMap);
	void SetHeightMap(Terrain::HeightMapType* m_heightMap);
	void SetEntityData(EntityData* entityData);
	struct BiomeObjectType
	{
		SimpleMath::Vector3 position;
		int modelID;
	};

	
	std::vector<BiomeObjectType> SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions, int terrainWidth);

	void AddToObjects(int modelID, int biomeType);
	
private:




	std::vector<int> m_desertObjects;
	std::vector<int> m_forestObjects;
	std::vector<int> m_snowObjects;
	EntityData* m_entityData;
	ModelClass snowModel, forestModel, desertModel;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, forestTexture, snowTexture;
	float currentCellWidth = 0;
	ClimateMap::ClimateMapType* m_climateMap;
	Terrain::HeightMapType* m_heightMap;
	
	int GetRandomObjectFromBiome(int biomeType);

	BiomeObjectType AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector3 climateClassification);
	BiomeObjectType SetupObject(int modelID,  SimpleMath::Vector3 position);




};

