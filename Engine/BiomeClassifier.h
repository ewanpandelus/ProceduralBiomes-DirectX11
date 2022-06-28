#pragma once
#include "PerlinNoise.h"
using namespace DirectX;

class BiomeClassifier
{
private:
	float rainfallMax, temperatureMax;
	struct BiomeType
	{
		float maxTemp;
		float minTemp;
		float maxRainfall;
		float minRainfall;
		
	};

	BiomeType m_biomes[2];

	BiomeType m_desert;
	BiomeType m_forest;

	std::vector<float> ClassifyBasedOnDistance(std::vector<float> distances);
	std::vector<float> RemoveDistanceFromUnclassifiedElements(std::vector<float> distances, int indexClassified);

	std::vector<float> CalculateFractionOfClosestBiomes(std::vector<float> sortedDistances);
public:
	void Initialise();
	float CalculateDistanceToBiome(float temp, float rainfall, BiomeType biome);
	SimpleMath::Vector2  CalculateDistanceToAllBiomes(float temp, float rainfall);

};



