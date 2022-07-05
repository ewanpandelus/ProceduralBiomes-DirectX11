#pragma once
#include "Model.h"
#include "modelclass.h"
#include "ClimateMap.h"
class BiomeObjects
{
public:
	void SetLargeModels(ModelClass desertModel, ModelClass forestModel, ModelClass snowModel);
	void SetLargeModelsTextures(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> forestTexture,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> snowTexture);
	SimpleMath::Vector2 ConvertPointToTerrainPosition(int x, int y);


	void SetCellWidth(float cellWidth);
	void SetClimateMap(ClimateMap::ClimateMapType* m_climateMap);
	struct BiomeObjectType
	{
		float x, z;
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
	BiomeObjectType AssignModelBasedOnClimate(float x, float z, SimpleMath::Vector2 climateClassification);
	BiomeObjectType SetupObject(ModelClass specificModel, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> associatedTexture,float x,float z, int id);


	
};

