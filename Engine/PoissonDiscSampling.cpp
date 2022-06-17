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



//bool PoissonDiscSampling::IsValid(SimpleMath::Vector2 candidate, SimpleMath::Vector2 sampleRegionSize, float cellSize, float radius, std::vector<SimpleMath::Vector2> points)
//{
//	if(candidate.x >= 0 && candidate.x < sampleRegionSize.x && candidate.y >=0 && candidate.y < sampleRegionSize.y)
//	{
//		int cellX = (int)(candidate.x / cellSize);
//		int cellY = (int)(candidate.y / cellSize);
//		int searchStartX = std::max(0, cellX - 2);
//		int searchEndX = std::min(cellX + 2, (int)(m_grid[0].size() - 1));
//		int searchStartY = std::max(0, cellY - 2);
//		int searchEndY = std::min(cellY + 2, (int)(m_grid.at(0).size() - 1));
//		for(int x = searchStartX; x<=searchEndX; x++)
//		{
//			for (int y = searchStartY; x <= searchEndY; x++)
//			{
//				int pointIndex = m_grid[x][y] - 1;
//				if(pointIndex!=-1)
//				{
//					float distance = (candidate - points[pointIndex]).LengthSquared();
//					SimpleMath::Vector2 difference = candidate - points[pointIndex];
//					if(distance<(radius*radius))
//					{
//						return false;
//					}
//				}
//			}
//		}
//		return true;
//	}
//	return false;
//}
std::vector<SimpleMath::Vector2> PoissonDiscSampling::GeneratePoints()
{
	std::vector<SimpleMath::Vector2> points;
	if (m_radius == 0 || m_sampleRegionLength == 0 || m_numSamplesBeforeRejection == 0) return points;
	float w = m_radius / sqrt(2); 
	int cols = floor(m_sampleRegionLength / w);
	int rows = floor(m_sampleRegionLength / w);
	
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
		int x = active.size();
		int r = rand() % x;
		
		SimpleMath::Vector2 position = active[r];
		bool candidateAccepted = false;
		for (int n = 0; n <m_numSamplesBeforeRejection;n++)
		{
			
			float angle = GenerateRandomFloatWithMaxVal(PI * 2);
			SimpleMath::Vector2 direction = SimpleMath::Vector2(cos(angle), sin(angle));
			float magnitude = GenerateRandomFloatWithMaxVal(m_radius) + m_radius;  // between 1-2 r;
			SimpleMath::Vector2 sample = position + direction * magnitude; //add random vector to point
			int col = floor(sample.x / w);
			int row = floor(sample.y / w);
			if (col <0 || col > m_grid.size()-1 || row  <0 || row > m_grid.size()-1) {
				continue;
			}
			bool ok = true;
			for(int i =-1;i<=1;i++)
			{
				for (int j = -1;j <= 1;j++)
				{
			
					if (col + i <0 || col + i > m_grid.size()-1 || row + j <0 || row + j > m_grid.size()-1) {
						continue;
					}
					SimpleMath::Vector2 neighbour = m_grid[col+i][row+j];
					if(neighbour != SimpleMath::Vector2(-1,-1))
					{
						float distance = (sample - neighbour).LengthSquared();
						if(distance < m_radius*m_radius)
						{
							ok = false;
						}
					}
				}
			}
			if(ok)
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




/*while(spawnPoints.size()>0)
	{
		int spawnIndex = rand() % spawnPoints.size();
		SimpleMath::Vector2 spawnCentre = spawnPoints[spawnIndex];
		bool candidateAccepted = false;
		for(int i = 0; i < numSamplesBeforeRejection; i++)
		{
			float random = rand() % 100;
			float angle = (random/ 100) * PI * 2;
			SimpleMath::Vector2 direction = SimpleMath::Vector2(Round1DP(sin(angle)), Round1DP(cos(angle)));
			float randUpToRadius = rand() % 100;
			randUpToRadius *= (radius/100);
			randUpToRadius = Round1DP(randUpToRadius);
			SimpleMath::Vector2 candidate = spawnCentre + direction * (radius + randUpToRadius);
			candidate = SimpleMath::Vector2(Round1DP(candidate.x), Round1DP(candidate.y));
			if(IsValid(candidate, sampleRegionSize, cellSize, radius, points))
			{
				points.push_back(candidate);
				spawnPoints.push_back(candidate);
				m_grid[(int)(candidate.x / cellSize)][(int)(candidate.y / cellSize)] = points.size();
				candidateAccepted = true;
				break;
			}
		}
		if (!candidateAccepted) {
			spawnPoints.erase(spawnPoints.begin() + spawnIndex);
		}
	}
	m_grid.empty();
	return points;*/
	/*for each (auto var in grid)
	{
		points.push_back(var);
	}
	return points;*/
