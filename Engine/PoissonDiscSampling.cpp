#include "pch.h"
#include "PoissonDiscSampling.h"



void PoissonDiscSampling::InitialiseGrid(int width, int height)
{
	for (int j = 0;j < width; j++) {
		std::vector<SimpleMath::Vector2> tmpGridIndex;
		for(int i = 0; i<height; i++)
		{
			tmpGridIndex.push_back(SimpleMath::Vector2(-1,-1));
		}
		m_grid.push_back(tmpGridIndex);
	}

}


std::vector<SimpleMath::Vector2> PoissonDiscSampling::GeneratePoints()
{
	std::vector<SimpleMath::Vector2> points;
	
	float w = m_radius / sqrt(2); 
	int cols = floor(m_sampleRegionLength / w);
	int rows = floor(m_sampleRegionLength / w);
	if (!CheckParametersValid(cols)) 
	{
		return points;
	}


	InitialiseGrid(cols, rows);
	std::vector<SimpleMath::Vector2> active;
	float x = m_sampleRegionLength/2;
	float y = m_sampleRegionLength /2;
	int i = floor(x) / w;
	int j = floor(y) / w;


	SimpleMath::Vector2 pos = SimpleMath::Vector2((x), (y));
	m_grid[i][j] = pos;
	active.push_back(pos);
	points.push_back(pos);


	while(active.size()>0)
	{
	
		int r = rand() % active.size();
		
		SimpleMath::Vector2 position = active[r];
		bool candidateAccepted = false;
		for (int n = 0; n <m_numSamplesBeforeRejection;n++)
		{
			SimpleMath::Vector2 sample = GenerateSample(position);
			int col = floor(sample.x / w);
			int row = floor(sample.y / w);


			if (col <0 || col > m_grid.size()-1 || row  <0 || row > m_grid.size()-1) {
				continue;
			}
			bool valid = CheckSamplingValid(sample, col, row);
			if(valid)
			{
				m_grid[col][row] = sample;
				candidateAccepted = true;
				active.push_back(sample);
				points.push_back(sample);
				break;
			}

			
		}
		if (!candidateAccepted)
		{
			active.erase(active.begin() + r);
		}
	}
	m_grid.clear();
	return points;
}

int* PoissonDiscSampling::GetSampleRegionSize()
{
	return &m_sampleRegionLength;
}

int* PoissonDiscSampling::GetNumSamplesBeforeRejection()
{
	return &m_numSamplesBeforeRejection;
}

float* PoissonDiscSampling::GetRadius()
{
	return &m_radius;
}

float PoissonDiscSampling::SquaredMagnitude(SimpleMath::Vector2 v)
{
	return (v.x*v.x) + (v.y*v.y);
}

float PoissonDiscSampling::Round1DP(float value)
{
	return ((float)((int)(value * 10))) / 10;
}

float PoissonDiscSampling::GenerateRandomFloatWithMaxVal(float value)
{
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * value;
}

bool PoissonDiscSampling::CheckParametersValid(int cols)
{
	return !(m_radius == 0 || m_sampleRegionLength == 0 || m_numSamplesBeforeRejection == 0 || cols == 0);
}

bool PoissonDiscSampling::CheckSamplingValid(SimpleMath::Vector2 sample, int col, int row)
{
	bool valid = true;
	for (int i = -1;i <= 1;i++)
	{
		for (int j = -1;j <= 1;j++)
		{

			if (col + i <0 || col + i > m_grid.size() - 1 || row + j <0 || row + j > m_grid.size() - 1) {
				continue;
			}
			SimpleMath::Vector2 neighbour = m_grid[col + i][row + j];
			if (neighbour != SimpleMath::Vector2(-1, -1))
			{
				float distance = (sample - neighbour).LengthSquared();
				if (distance < m_radius * m_radius)
				{
					valid = false;
				}
			}
		}
	}
	return valid;
}

SimpleMath::Vector2 PoissonDiscSampling::GenerateSample(SimpleMath::Vector2 position)
{
	float angle = GenerateRandomFloatWithMaxVal(PI * 2);
	SimpleMath::Vector2 direction = SimpleMath::Vector2(cos(angle), sin(angle));
	float magnitude = GenerateRandomFloatWithMaxVal(m_radius) + m_radius;  // between 1-2 r;
	return  position + direction * magnitude; //add random vector to point
}





