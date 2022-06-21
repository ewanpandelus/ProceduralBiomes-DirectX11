#pragma once
class DepthTexture
{
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DepthTexture::GenerateDepthTexture(ID3D11Device* device);
};

