#pragma once
#include "PerlinNoise.h"
#include "BiomeClassifier.h"
#include <map>
using std::map;
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
	float* GetPositionalOffsetX();
	float* GetPositionalOffsetZ();
	struct ClimateMapType
	{
		float x, z;
		float temperature;
		float rainfall;
		SimpleMath::Vector3 climateClassification;
	};


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateClimateMapTexture(ID3D11Device* device, ClimateMapType* climateMap);
	ClimateMapType* GetClimateMap();
	ClimateMapType GenerateClimateMapIndvidual();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateNoiseTexture(ID3D11Device* device);
	uint32_t RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b);
	ClimateMapType* GenerateClimateMap(int xOffset, int zOffset);
	map<SimpleMath::Vector2, ClimateMapType*> GenerateClimateMaps(int positionalStep, SimpleMath::Vector2 playerPos);
private:
	int m_tempGridWidth, m_tempGridHeight;
	float m_maxTemp = -1000, m_minTemp = 1000;
	float m_maxRainfall = -1000, m_minRainfall = 1000;

	float m_temperatureAmplitude = 1, m_temperatureFrequency = 0.012, m_temperatureOffset = 51.282;
	float m_rainfallAmplitude = 1, m_rainFallFrequency = 0.012, m_rainfallOffset = 36.4;
	float m_positionalOffsetX = 0, m_positionalOffsetZ = 0;
	BiomeClassifier m_biomeClassifier;

	PerlinNoise m_perlinNoise;

	ClimateMapType* m_climateMap;
	map<SimpleMath::Vector2, ClimateMapType*> m_climateMaps;
	const int RED = 255; const int GREEN = 255; const int BLUE = 0; const int ALPHA = 255;
	float InverseLerp(float u, float v, float value);
	void AssessMaxAndMinNoiseValues(float noiseVal, float* maxNoise, float* minNoise);

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> CreateShaderResources(ID3D11Device* device, std::vector<uint32_t> colourBuffers);


	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> CreateShaderResources(ID3D11Device device, std::vector<uint32_t> colourBuffers);

};



