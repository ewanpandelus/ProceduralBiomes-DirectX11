#include "pch.h"
#include "EntityData.h"

int EntityData::AddToMap(ModelClass model) {
	entities.insert({ currentID, model });
	currentID++;
	return currentID - 1;
}
void EntityData::IncreaseEntityCount(int modelID, DirectX::SimpleMath::Vector3 position) {

    if (position.y > 40 || position.y <0.4) {
        return;
    }
    float percentage = rand() % 101;
    if (percentage > entities[modelID].GetPlacementPercentage()) {
        return;
    }
    totalCount++;
	entities[modelID].AddNewPosition(position);
}

std::vector<ModelClass> EntityData::GetModels()
{
    std::vector<ModelClass> models;
    auto iter = entities.begin();
    int index = 0;
    while (iter != entities.end()) {
        models.push_back(entities[index]);
        ++iter;
        index++;
    }
    return models;
}

void EntityData::SetupModelBuffers(ID3D11Device* device)
{
    auto iter = entities.begin();
    int index = 0;
    while (iter != entities.end()) {
        entities[index].InitializeBuffers(device);
        ++iter;
        index++;
    }
}

void EntityData::ClearModelBuffers()
{
    totalCount = 0;
    auto iter = entities.begin();
    int index = 0;
    while (iter != entities.end()) {
        entities[index].ClearInstances();
        ++iter;
        index++;
    }
}


