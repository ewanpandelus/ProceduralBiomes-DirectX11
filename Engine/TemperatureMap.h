#pragma once
#include "PerlinNoise.h"
using namespace DirectX;

class TemperatureMap
{
private:
	int m_tempGridWidth, m_tempGridHeight;
	float m_maxNoise, m_minNoise;
	float m_frequency = 0, m_amplitude = 0, m_offset = 0;
	
	PerlinNoise perlinNoise;
	struct TemperatureMapType
	{
		float x, z;
		float temperature;
	};
	TemperatureMapType* m_temperatureMap;
	const int RED = 255; const int GREEN = 255; const int BLUE = 0; const int ALPHA = 255;
	float InverseLerp(float u, float v, float value);
	void AssessMaxAndMinNoiseValues(float noiseVal);
public:
	void Initialize(int tempGridWidth, int tempGridHeight);
	void GenerateTemperatureMap();
	float* GetOffset();
	float* GetFrequency();
	float* GetAmplitude();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GenerateNoiseTexture(ID3D11Device* device);
	uint32_t RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b);

	
	
};



