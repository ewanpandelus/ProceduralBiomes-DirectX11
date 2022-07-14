#pragma once
using namespace DirectX;
#include "BiomeObjects.h"

class PoissonDiscSampling
{
# define PI           3.141592
public:
	PoissonDiscSampling() {}
	PoissonDiscSampling(BiomeObjects biomeObjects)
		: m_biomeObjects(biomeObjects)
	{}
	void InitialiseGrid(int width, int height);
	std::vector<SimpleMath::Vector2> GeneratePoints();
	int* GetSampleRegionSize();
	int* GetNumSamplesBeforeRejection();
	float* GetRadius();
	float GetCellWidth();
private:

	float cellWidth = 0;
	float SquaredMagnitude(SimpleMath::Vector2 v);
	float Round1DP(float value);
	BiomeObjects m_biomeObjects;

	std::vector<std::vector<SimpleMath::Vector2>> m_grid;
	float m_radius = 0;
	int m_numSamplesBeforeRejection = 30;
	int m_sampleRegionLength = 63;

	float GenerateRandomFloatWithMaxVal(float value);
	bool CheckParametersValid(int cols);
	bool CheckSamplingValid(SimpleMath::Vector2 sample, int col, int row);
	SimpleMath::Vector2 GenerateSample(SimpleMath::Vector2 position);
	

};

