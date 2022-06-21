#include "pch.h"
#include "TemperatureMap.h"





void TemperatureMap::Initialize(int tempGridWidth, int tempGridHeight)
{
	std::vector<uint32_t> m_colourBuffers(128 * 128);
	perlinNoise.Initialize();
	int index;
	m_tempGridWidth = tempGridWidth;
	m_tempGridHeight = tempGridHeight;
	m_temperatureMap = new TemperatureMapType[m_tempGridWidth * m_tempGridHeight];
	for (int j = 0; j < m_tempGridHeight; j++)
	{
		for (int i = 0; i < m_tempGridWidth; i++)
		{
			index = (m_tempGridHeight * j) + i;
			m_temperatureMap[index].x = (float)i;
			m_temperatureMap[index].z = (float)j;
		}
	}
}
void TemperatureMap::GenerateTemperatureMap()
{
    m_maxNoise = -1000;
	m_minNoise = 1000;
	int index = 0;
	for (int j = 0; j < m_tempGridHeight; j++)
	{
		for (int i = 0; i < m_tempGridWidth; i++)
		{ 
			index = (m_tempGridHeight * j) + i;
	
			float perlinValue = (float)perlinNoise.Noise((i * m_frequency)+m_offset, (j * m_frequency+m_offset), 1);
			m_temperatureMap[index].temperature = perlinValue*m_amplitude;
			AssessMaxAndMinNoiseValues(perlinValue);  //If the max and min are influenced by ampltude then the range never changes 
		}
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TemperatureMap::GenerateNoiseTexture(ID3D11Device* device)
{
	//int width = m_tempGridWidth;
	int width = 128;
	std::vector<uint32_t> m_colourBuffers(width * width);

	int index = 0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texSRV;
	{
	
		for (int i = 0; i < width; i++) {
			for(int j = 0;j < width; j++)
			{
				index = ((width * j) + i);
				int rgbValue = (InverseLerp(m_minNoise, m_maxNoise, m_temperatureMap[index].temperature) * 255);
				m_colourBuffers.at(index) = RGB_TO_UNSIGNED_INT_COLOUR(rgbValue, rgbValue, rgbValue);


			}

		}
	
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = width;
		textureDesc.MipLevels = 1; /// !!!
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA tSData = { &m_colourBuffers, sizeof(m_colourBuffers), 0 };
		tSData.pSysMem = &(m_colourBuffers[0]);
		 int bpp = 32;
		size_t rowPitch = (width * bpp + 7) / 8;
		size_t imageSize = rowPitch * 128;
		tSData.SysMemPitch = static_cast<UINT>(rowPitch);
		tSData.SysMemSlicePitch = static_cast<UINT>(imageSize);
		

		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	
		HRESULT hr = device->CreateTexture2D(&textureDesc, &tSData, tex.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(tex.Get(),
				&SRVDesc, texSRV.GetAddressOf());
		}

		if (FAILED(hr))
		{
			tex->Release();
		}
		
	}
	return texSRV;





	

	
}
uint32_t TemperatureMap::RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b)
{
	return  r | (g << 8) | (b << 16) | (ALPHA << 24);
}
float TemperatureMap::InverseLerp(float u, float v, float value)
{
	
	float inverseLerp = (value - u) / (v - u);
	if (inverseLerp < 0) return 0;
	if (inverseLerp > 1) return 1;
	return inverseLerp;   //Compensates for values which end up outside the range as the amplitude is ignored
	
}
void TemperatureMap::AssessMaxAndMinNoiseValues(float noiseVal)
{
	if (noiseVal > m_maxNoise) {
		m_maxNoise = noiseVal;
		return;
	}
	if (noiseVal < m_minNoise) {
		m_minNoise = noiseVal;
	}
}
float* TemperatureMap::GetAmplitude()
{
	return &m_amplitude;
}
float* TemperatureMap::GetFrequency()
{
	return &m_frequency;
}
float* TemperatureMap::GetOffset()
{
	return &m_offset;
}

