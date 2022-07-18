#pragma once
#include <map>
#include "modelclass.h"
using std::map;
class EntityData
{
public:
	int AddToMap(ModelClass model);
	void IncreaseEntityCount(int modelID, DirectX::SimpleMath::Vector3 position);
	std::vector<ModelClass> GetModels();
	void SetupModelBuffers(ID3D11Device*);
	void ClearModelBuffers();
private:
	int currentID = 0;
	map<int, ModelClass> entities;

	
};

