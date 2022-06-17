#pragma once
using namespace DirectX;

class PoissonDiscSampling
{
# define PI           3.141592
public:
	void InitialiseGrid(int width, int height);
	std::vector<SimpleMath::Vector2> GeneratePoints();
	int* GetSampleRegionSize();
	int* GetNumSamplesBeforeRejection();
	float* GetRadius();
private:
	bool IsValid(SimpleMath::Vector2 candidate, SimpleMath::Vector2 sampleRegionSize, float cellSize, float radius, std::vector<SimpleMath::Vector2> points);

	float SquaredMagnitude(SimpleMath::Vector2 v);
	float Round1DP(float value);


	std::vector<std::vector<SimpleMath::Vector2>> m_grid;
	float m_radius = 0;
	int m_numSamplesBeforeRejection = 0;
	int m_sampleRegionLength = 0;

	float GenerateRandomFloatWithMaxVal(float value);

};

