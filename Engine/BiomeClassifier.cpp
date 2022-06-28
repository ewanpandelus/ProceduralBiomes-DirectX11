#include "pch.h"
#include "BiomeClassifier.h"

void BiomeClassifier::Initialise()
{
    temperatureMax = rainfallMax = 40;



    m_desert.maxTemp = 40;
    m_desert.minTemp = 30;
    m_desert.maxRainfall = 10;
    m_desert.minRainfall = 0;



    m_forest.maxTemp = 30;
    m_forest.minTemp = 10;
    m_forest.maxRainfall = 30;
    m_forest.minRainfall = 15;
   

    m_biomes[0] = m_desert;
    m_biomes[1] = m_forest;
}


std::vector<float> BiomeClassifier::ClassifyBasedOnDistance(std::vector<float> distances)
{
   std::vector<float> sortedDistances;
   for(int i = 0; i< 2; i++)
   {
       sortedDistances.push_back(distances[i]);
   }
   std::sort(sortedDistances.begin(), sortedDistances.end());
 
 
   std::vector<float> fractions = CalculateFractionOfClosestBiomes(sortedDistances);
   for (int i = 0; i < 2; i++)
   {
       for(int index = 0; index<2; index++)
       {
           if(sortedDistances[i] == distances[index])
           {
               distances[index] = fractions[i];
           }
       }
   }
   return distances;
}

float BiomeClassifier::CalculateDistanceToBiome(float temp, float rainfall, BiomeType biome)

{
    temp *= temperatureMax;
    rainfall *= rainfallMax;

    float distance = 0;
    bool withinBoundsTemp = (temp >= biome.minTemp && temp <= biome.maxTemp);     
    bool withinBoundsRainfall = (rainfall >= biome.minRainfall && rainfall <= biome.maxRainfall);                    

    distance += withinBoundsTemp ? 0 : std::min(abs(temp - biome.minTemp), abs(temp - biome.maxTemp)); // if not in bounds check distance to closest bound and add to total
    distance += withinBoundsRainfall ? 0 : std::min(abs(rainfall - biome.minRainfall), abs(rainfall - biome.maxRainfall));   

    return distance;
}

std::vector<float> BiomeClassifier::RemoveDistanceFromUnclassifiedElements(std::vector<float> distances, int indexClassified)
{
    for (int i = 0; i < 2; i++) {
        if (i != indexClassified) {
            distances[i] = 0;
        }
        else {
            break;
        }
    }
    return distances;
}

SimpleMath::Vector2 BiomeClassifier::CalculateDistanceToAllBiomes(float temp, float rainfall)
{
    std::vector<float> distances(2);
     int indexClassified = -1;
     for(int i = 0; i<2 ;i++)
     {
         distances[i] = CalculateDistanceToBiome(temp, rainfall, m_biomes[i]);
         if (distances[i] == 0) {
             distances[i] = 1;
             indexClassified = i;
             break;
         }
     }
     if(indexClassified!=-1)
     {
         distances =  RemoveDistanceFromUnclassifiedElements(distances, indexClassified);
        
         return SimpleMath::Vector2(distances[0], distances[1]);
     }
     distances = ClassifyBasedOnDistance(distances);
     SimpleMath::Vector2 biomeDistances = SimpleMath::Vector2(distances[0], distances[1]);
     return biomeDistances;
}
std::vector<float> BiomeClassifier::CalculateFractionOfClosestBiomes(std::vector<float> sortedDistances)
{
    std::vector<float> fractions(2);
    float total = sortedDistances[0] + sortedDistances[1];
    fractions[0] = sortedDistances[1] / total;                         //Must be opposite fraction if closer to biome (i.e. distance to forest 4, distance to desert 2, then 4/6 desert, 66.66% desert
    fractions[1] = sortedDistances[0] / total;
    return fractions;
}








