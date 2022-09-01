#pragma once

#include "DeviceResources.h"
#include "Light.h"
#include "Shader.h"
//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class WaterShader : Shader
{
public:
	WaterShader();
	~WaterShader();
	void InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);
    bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);		//Loads the Vert / pixel Shader pair
	bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection, Light* sceneLight1, ID3D11ShaderResourceView* texture1, float time, SimpleMath::Vector3 cameraPosition);
    void EnableShader(ID3D11DeviceContext* context, bool geomOn);



};