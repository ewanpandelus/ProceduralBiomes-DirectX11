#include "pch.h"
#include "BiomeClassifier.h"

void BiomeClassifier::Initialise()
{
    temperatureMax = rainfallMax = 60;



    m_desert.maxTemp = 60;
    m_desert.minTemp = 40;
    m_desert.maxRainfall = 20;
    m_desert.minRainfall = 0;



    m_forest.maxTemp = 30;
    m_forest.minTemp = 25;
    m_forest.maxRainfall = 30;
    m_forest.minRainfall = 25;


    m_snow.maxTemp = 10;
    m_snow.minTemp = 0;
    m_snow.maxRainfall = 60;
    m_snow.minRainfall = 47;


    m_biomes[0] = m_desert;
    m_biomes[1] = m_forest;
    m_biomes[2] = m_snow;
}


std::vector<float> BiomeClassifier::ClassifyBasedOnDistance(std::vector<float> distances)
{



    std::vector<float> sortedDistances;
    for (int i = 0; i < 3; i++)
    {
        sortedDistances.push_back(distances[i]);
    }
    std::sort(sortedDistances.begin(), sortedDistances.end());


    std::vector<float> fractions = CalculateFractionOfClosestBiomes(sortedDistances);
    for (int i = 0; i < 3; i++)
    {
        for (int index = 0; index < 3; index++)
        {
            if (sortedDistances[i] == distances[index])
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

    distance += withinBoundsTemp ? 0 : std::min(pow((temp - biome.minTemp), 2), pow(temp - biome.maxTemp, 2)); // if not in bounds check distance to closest bound and add to total
    distance += withinBoundsRainfall ? 0 : std::min(pow(rainfall - biome.minRainfall, 2), pow(rainfall - biome.maxRainfall, 2));

    return distance;
}

std::vector<float> BiomeClassifier::RemoveDistanceFromUnclassifiedElements(std::vector<float> distances, int indexClassified)
{
    for (int i = 0; i < 3; i++) {
        if (i != indexClassified) {
            distances[i] = 0;
        }
        else {
            break;
        }
    }
    return distances;
}

SimpleMath::Vector3 BiomeClassifier::CalculateDistanceToAllBiomes(float temp, float rainfall)
{
    std::vector<float> distances(3);

    int indexClassified = -1;
    for (int i = 0; i < 3;i++)
    {
        distances[i] = CalculateDistanceToBiome(temp, rainfall, m_biomes[i]);
        if (distances[i] <= 0) {
            distances[i] = 1;
            indexClassified = i;
            break;
        }
    }
    if (distances[0] < distances[1] && distances[2] < distances[1]) {
        int x = 4;
    }
    if (indexClassified != -1)
    {
        distances = RemoveDistanceFromUnclassifiedElements(distances, indexClassified);

        return SimpleMath::Vector3(distances[0], distances[1], distances[2]);
    }

    distances = ClassifyBasedOnDistance(distances);
    return SimpleMath::Vector3(distances[0], distances[1], distances[2]);
}
std::vector<float> BiomeClassifier::CalculateFractionOfClosestBiomes(std::vector<float> sortedDistances)
{
    std::vector<float> fractions(3);
    float frac1 = sortedDistances[0];
    float frac2 = sortedDistances[1];


    frac1 = ExponentialFraction(frac1);         //Creates quicker changes between biomes
    frac2 = ExponentialFraction(frac2);


    float total = frac1 + frac2;
    fractions[0] = frac2 / total;               //Must be opposite fraction if closer to biome (i.e. distance to forest 4, distance to desert 2, then 4/6 desert, 66.66% desert
    fractions[1] = frac1 / total;



    fractions[2] = 0;
    //return fractions;



    return fractions;
}

float  BiomeClassifier::ExponentialFraction(float expFrac) {
    return pow((expFrac * 10), 2);
}








