////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct InstanceType
	{
		DirectX::SimpleMath::Vector3 position;
		float scale;
	};
public:
	ModelClass();
	~ModelClass();

	bool InitializeModel(ID3D11Device* device, char* filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void IndexRender(ID3D11DeviceContext*);

	int GetIndexCount();
	int GetVertexCount();
	int GetInstanceCount();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture();
	void AddNewPosition(DirectX::SimpleMath::Vector3 position);
	bool InitializeBuffers(ID3D11Device*);
	void ClearInstances();
	float GetPlacementPercentage();
	void SetPlacementPercentage(float placementPercentage);
	
private:

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);
	void GenerateRandomScale();
	void ReleaseModel();

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Buffer* m_instanceBuffer;
	float m_placementPercentage = 100;
	int m_vertexCount, m_indexCount, m_faceSize;
	int m_instanceCount = 0;
	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   m_texture;
	int												   m_biomeType;
	std::vector<SimpleMath::Vector3> m_positions;
	std::vector<float> m_scales;

};

#endif