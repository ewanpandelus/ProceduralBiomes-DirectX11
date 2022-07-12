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

	struct ObjectType
	{
		ModelClass model;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	};
	std::vector<BiomeObjectType> SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions);

	void AddToObjects(ModelClass model, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, int biomeType);
	
private:




	std::vector<ObjectType> m_desertObjects;
	std::vector<ObjectType> m_forestObjects;
	std::vector<ObjectType> m_snowObjects;

	ModelClass snowModel, forestModel, desertModel;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> desertTexture, forestTexture, snowTexture;
	float currentCellWidth = 0;
	ClimateMap::ClimateMapType* m_climateMap;
	Terrain::HeightMapType* m_heightMap;
	
	ObjectType GetRandomObjectFromBiome(int biomeType);

	BiomeObjectType AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector3 climateClassification);
	BiomeObjectType SetupObject(ModelClass model, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, SimpleMath::Vector3 position);




};

