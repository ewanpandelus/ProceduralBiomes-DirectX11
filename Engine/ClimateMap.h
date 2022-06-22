#pragma once
#include "PerlinNoise.h"
using namespace DirectX;

class ClimateMap
{
private:
	int m_tempGridWidth, m_tempGridHeight;
	float m_maxTemp, m_minTemp;
	float m_maxRainfall, m_minRainfall;

	float m_temperatureAmplitude = 0, m_temperatureFrequency = 0, m_temperatureOffset = 0;
	float m_rainfallAmplitude = 0, m_rainFallFrequency = 0, m_rainfallOffset= 0;

	
	PerlinNoise perlinNoise;
	struct ClimateMapType
	{
		float x, z;
		float temperature;
		float rainfall;
	};
	ClimateMapType* m_climateMap;

	const int RED = 255; const int GREEN = 255; const int BLUE = 0; const int ALPHA = 255;
	float InverseLerp(float u, float v, float value);
	void AssessMaxAndMinNoiseValues(float noiseVal, float* maxNoise, float* minNoise);
public:
	void Initialize(int tempGridWidth, int tempGridHeight);
	void GenerateClimateMap();
	float* GetTemperatureOffset();
	float* GetTemperatureFrequency();
	float* GetTemperatureAmplitude();
	float* GetRainfallOffset();
	float* GetRainfallFrequency();
	float* GetRainfallAmplitude
();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateNoiseTexture(ID3D11Device* device, char* noiseMap);
	uint32_t RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b);

	
	
};



