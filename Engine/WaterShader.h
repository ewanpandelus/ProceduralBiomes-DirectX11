#pragma once

#include "DeviceResources.h"
#include "Light.h"
#include "ClimateMap.h"
#include "Shader.h"

//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class WaterShader : public Shader
{
public:
	WaterShader();
	~WaterShader();

	//we could extend this to load in only a vertex shader, only a pixel shader etc.  or specialised init for Geometry or domain shader. 
	//All the methods here simply create new versions corresponding to your needs
	void InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);		//Loads the Vert / pixel Shader pair
	bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1, 
		float maxDepth);
	void EnableShader(ID3D11DeviceContext* context);
	



private:


	//buffer to pass in camera world Position
	struct CameraBufferType
	{
		DirectX::SimpleMath::Vector3 cameraPosition;
		float padding;
	};

	
};

