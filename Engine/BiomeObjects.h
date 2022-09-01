#pragma once
#include "Model.h"
#include "modelclass.h"
#include "ClimateMap.h"
#include "Terrain.h" 
#include "EntityData.h"
#include "BarycentricCoordinates.h"
class BiomeObjects
{
public:
	

	



	void SetCellWidth(float cellWidth);
	void SetClimateMap(ClimateMap::ClimateMapType* m_climateMap);
	void SetBarycentricCoordinates(BarycentricCoordinates* barycentricCoords);
	
	void SetEntityData(EntityData* entityData);
	struct BiomeObjectType
	{
		SimpleMath::Vector3 position;
		int modelID;
	};

	
	std::vector<BiomeObjectType> SetupObjectsAccordingToBiomes(std::vector<SimpleMath::Vector2> gridPositions, int terrainWidth, int terrainScale);

	void AddToObjects(int modelID, int biomeType);
	void SetIsSmall(bool isSmall);
	
private:




	std::vector<int> m_desertObjectsSmall;
	std::vector<int> m_desertObjectsLarge;
	std::vector<int> m_forestObjectsSmall;
	std::vector<int> m_forestObjectsLarge;
	std::vector<int> m_snowObjectsSmall;
	std::vector<int> m_snowObjectsLarge;
	std::vector<int> m_waterObjectsSmall;
	std::vector<int> m_waterObjectsLarge;
	EntityData* m_entityData;

	float currentCellWidth = 0;
	ClimateMap::ClimateMapType* m_climateMap;
	BarycentricCoordinates* m_barycentricCoords;
	int GetRandomObjectFromBiome(int biomeType);
	bool isSmall = false;

	BiomeObjectType AssignModelBasedOnClimate(SimpleMath::Vector3 position, SimpleMath::Vector4 climateClassification);
	BiomeObjectType SetupObject(int modelID,  SimpleMath::Vector3 position);




};

