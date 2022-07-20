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

void PoissonDiscSampling::GenerateAllPoints(int numSamplesBeforeRejectionBig, float radiusBig, int numSamplesBeforeRejectionSmall, float radiusSmall)
{
	m_bigObjPoints.clear();
	m_smallObjPoints.clear();
	m_savedPoints.clear();
	m_numSamplesBeforeRejection = numSamplesBeforeRejectionBig;
	m_radius = radiusBig;
    m_bigObjPoints = GeneratePoints();
	m_numSamplesBeforeRejection = numSamplesBeforeRejectionSmall;
	m_radius = radiusSmall;
	m_smallObjPoints = GeneratePoints();
}

std::vector<SimpleMath::Vector2> PoissonDiscSampling::GetBigObjPoints()
{
	return m_bigObjPoints;
}

std::vector<SimpleMath::Vector2> PoissonDiscSampling::GetSmallObjPoints()
{
	return m_smallObjPoints;
}

std::vector<SimpleMath::Vector2> PoissonDiscSampling::GeneratePoints()
{
	std::vector<SimpleMath::Vector2> points;
	
	cellWidth = m_radius / sqrt(2); 
	m_biomeObjects.SetCellWidth(cellWidth);
	int cols = floor(m_sampleRegionLength / cellWidth);
	int rows = floor(m_sampleRegionLength / cellWidth);
	if (!CheckParametersValid(cols)) 
	{
		return points;
	}
	float min = 1000;
	float maximum = -1000;

	InitialiseGrid(cols, rows);

//	m_grid = PopulateGrid(m_grid, cellWidth, m_savedPoints);
	
	std::vector<SimpleMath::Vector2> active;
	float x = m_sampleRegionLength/2;
	float y = m_sampleRegionLength /2;
	int i = floor(x) / cellWidth;
	int j = floor(y) / cellWidth;


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
			int col = floor(sample.x / cellWidth);
			int row = floor(sample.y / cellWidth);


			if (col <0 || col > m_grid.size()-1 || row  <0 || row > m_grid.size()-1) {
				continue;
			}
			bool valid = CheckSamplingValid(sample, col, row);
			if(valid)
			{
				m_grid[col][row] = sample;
				candidateAccepted = true;
				if (sample.x > maximum || sample.y > maximum) {
					maximum = std::max(sample.x, sample.y);
				}
				if (sample.x < min || sample.y < min) {
					min = std::min(sample.x, sample.y);
				}
				active.push_back(sample);
				points.push_back(sample);
				m_savedPoints.push_back(sample);
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

float PoissonDiscSampling::GetCellWidth()
{
	return cellWidth;
}

void PoissonDiscSampling::ClearSavedPoints()
{
	m_savedPoints.clear();
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

std::vector<std::vector<SimpleMath::Vector2>> PoissonDiscSampling::PopulateGrid(std::vector<std::vector<SimpleMath::Vector2>> grid, float cellWidth, std::vector<SimpleMath::Vector2> points)
{
	for each(auto point in points) {
		int col = floor(point.x / cellWidth);
		int row = floor(point.y / cellWidth);
		grid[col][row] = point;
	}
	return grid;
}

SimpleMath::Vector2 PoissonDiscSampling::GenerateSample(SimpleMath::Vector2 position)
{
	float angle = GenerateRandomFloatWithMaxVal(PI * 2);
	SimpleMath::Vector2 direction = SimpleMath::Vector2(cos(angle), sin(angle));
	float magnitude = GenerateRandomFloatWithMaxVal(m_radius) + m_radius;  // between 1-2 r;
	return  position + direction * magnitude; //add random vector to point
}







