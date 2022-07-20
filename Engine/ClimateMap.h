#pragma once
#include "PerlinNoise.h"
#include "BiomeClassifier.h"

using namespace DirectX;

class ClimateMap
{

public:

	void Initialize(int tempGridWidth, int tempGridHeight);

	float* GetTemperatureOffset();
	float* GetTemperatureFrequency();
	float* GetTemperatureAmplitude();
	float* GetRainfallOffset();
	float* GetRainfallFrequency();
	float* GetRainfallAmplitude();
	struct ClimateMapType
	{
		float x, z;
		float temperature;
		float rainfall;
		SimpleMath::Vector3 climateClassification;
	};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateClimateMapTexture(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateNoiseTexture(ID3D11Device* device);
	uint32_t RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b);
	ClimateMapType* GenerateClimateMap();
private:
	int m_tempGridWidth, m_tempGridHeight;
	float m_maxTemp, m_minTemp;
	float m_maxRainfall, m_minRainfall;

	float m_temperatureAmplitude = 1, m_temperatureFrequency = 0.008, m_temperatureOffset = 30.137;
	float m_rainfallAmplitude = 1, m_rainFallFrequency = 0.025, m_rainfallOffset = 84.247;

	BiomeClassifier m_biomeClassifier;

	PerlinNoise m_perlinNoise;

	ClimateMapType* m_climateMap;

	const int RED = 255; const int GREEN = 255; const int BLUE = 0; const int ALPHA = 255;
	float InverseLerp(float u, float v, float value);
	void AssessMaxAndMinNoiseValues(float noiseVal, float* maxNoise, float* minNoise);




};



