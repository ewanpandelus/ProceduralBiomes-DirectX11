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

	BiomeType m_biomes[4];

	BiomeType m_desert;
	BiomeType m_forest;
	BiomeType m_snow;
	BiomeType m_water;

	std::vector<float> ClassifyBasedOnDistance(std::vector<float> distances);
	std::vector<float> RemoveDistanceFromUnclassifiedElements(std::vector<float> distances, int indexClassified);
	float ExponentialFraction(float expFrac);
	std::vector<float> CalculateFractionOfClosestBiomes(std::vector<float> sortedDistances);
public:
	void Initialise();
	float CalculateDistanceToBiome(float temp, float rainfall, BiomeType biome);
	SimpleMath::Vector4  CalculateDistanceToAllBiomes(float temp, float rainfall);

};



