#pragma once
#include "Model.h"
#include "modelclass.h"
#include "ClimateMap.h"
#include "Terrain.h" 
class BiomeObjects
{
public:
	void SetLargeModels(ModelClass desertModel, ModelClass forestModel, ModelClass snowModel);
	void SetLargeModelsTextures(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> forestTexture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> snowTexture);
	SimpleMath::Vector2 ConvertPointToTerrainPosition(int x, int y);


	void SetCellWidth(float cellWidth);
	void SetClimateMap(ClimateMap::ClimateMapType* m_climateMap);
	void SetHeightMap(Terrain::HeightMapType* m_heightMap);

	struct BiomeObjectType
	{
		SimpleMath::Vector3 position;
		ModelClass model;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	};
	std::vector<BiomeObjectType> SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions);
private:
	
	bool PickFirstChoice(float percentage, float firstPercentage);
	
	ModelClass snowModel, forestModel, desertModel;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, forestTexture, snowTexture;
	float currentCellWidth = 0;
	ClimateMap::ClimateMapType* m_climateMap;
	Terrain::HeightMapType* m_heightMap;


	BiomeObjectType AssignModelBasedOnClimate(SimpleMath::Vector3 position,  SimpleMath::Vector3 climateClassification);
	BiomeObjectType SetupObject(ModelClass specificModel, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> associatedTexture, SimpleMath::Vector3 position);


	
};

