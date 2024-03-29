#pragma once

#include "DeviceResources.h"
#include "Light.h"
#include "ClimateMap.h"
#include "Shader.h"

//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class TerrainShader : public Shader
{
public:
	TerrainShader();
	~TerrainShader();

	//we could extend this to load in only a vertex shader, only a pixel shader etc.  or specialised init for Geometry or domain shader. 
	//All the methods here simply create new versions corresponding to your needs
	void InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);		//Loads the Vert / pixel Shader pair
	bool SetBiomeShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1,
		ID3D11ShaderResourceView* noiseTemperatureTexture,  ID3D11ShaderResourceView* noiseTexture);
	void EnableShader(ID3D11DeviceContext* context);
	float* SetForestColour();
	std::vector<float> GetForestColour();
	float* SetDesertColour();
	std::vector<float> GetDesertColour();
	float* SetSnowColour();
	std::vector<float> GetSnowColour();




private:

	float m_desertColour[3] = { 0.95, 0.77, 0.5 };
	float m_forestColour[3] = { 0.2, 0.67, 0.335 };
	float m_snowColour[3] = { 1,1,1 };

	//buffer to pass in camera world Position
	struct CameraBufferType
	{
		DirectX::SimpleMath::Vector3 cameraPosition;
		float padding;
	};
	struct NoiseTextureBufferType
	{
		float frequency;
		float amplitude;
		float offset;
		float excess1;
	};
	struct BiomeColourBufferType
	{
		DirectX::SimpleMath::Vector4 desertColour;
		DirectX::SimpleMath::Vector4 forestColour;
		DirectX::SimpleMath::Vector3 snowColour;
		float excess2;
	};

	//Shaders
	ID3D11Buffer* m_noiseTextureBuffer;
	ID3D11Buffer* m_biomeColourBuffer;
};

