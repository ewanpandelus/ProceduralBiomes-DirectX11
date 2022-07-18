#include "pch.h"
#include "ClimateMap.h"






void ClimateMap::Initialize(int tempGridWidth, int tempGridHeight)
{

	std::vector<uint32_t> m_colourBuffers(tempGridWidth * tempGridHeight);
	m_perlinNoise.Initialize();
	m_biomeClassifier.Initialise();
	int index;
	m_tempGridWidth = tempGridWidth;
	m_tempGridHeight = tempGridHeight;
	m_climateMap = new ClimateMapType[m_tempGridWidth * m_tempGridHeight];
	for (int j = 0; j < m_tempGridHeight; j++)
	{
		for (int i = 0; i < m_tempGridWidth; i++)
		{
			index = (m_tempGridHeight * j) + i;
			m_climateMap[index].x = (float)i;
			m_climateMap[index].z = (float)j;
		}
	}
}

ClimateMap::ClimateMapType* ClimateMap::GenerateClimateMap()
{
	m_maxTemp = -1000;
	m_minTemp = 1000;
	m_maxRainfall = -1000;
	m_minRainfall = 1000;

	int index = 0;
	for (int j = 0; j < m_tempGridHeight; j++)
	{
		for (int i = 0; i < m_tempGridWidth; i++)
		{
			index = (m_tempGridHeight * j) + i;

			float tempPerlinVal = (float)m_perlinNoise.Noise((i * m_temperatureFrequency) + m_temperatureOffset, (j * m_temperatureFrequency + m_temperatureOffset), 1);
			m_climateMap[index].temperature = tempPerlinVal * m_temperatureAmplitude;

			float rainfallPerlinVal = (float)m_perlinNoise.Noise((i * m_rainFallFrequency) + m_rainfallOffset, (j * m_rainFallFrequency + m_rainfallOffset), 1);
			m_climateMap[index].rainfall = rainfallPerlinVal * m_rainfallAmplitude;


			AssessMaxAndMinNoiseValues(tempPerlinVal, &m_maxTemp, &m_minTemp);  //If the max and min are influenced by ampltude then the range never changes 
			AssessMaxAndMinNoiseValues(rainfallPerlinVal, &m_maxRainfall, &m_minRainfall);

		}
	}
	return m_climateMap;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ClimateMap::GenerateClimateMapTexture(ID3D11Device* device)
{
	int resolution = m_tempGridWidth;
	std::vector<uint32_t> m_colourBuffers(resolution * resolution);

	int index = 0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texSRV;
	{

		for (int i = 0; i < resolution; i++) {
			for (int j = 0;j < resolution; j++)
			{
				index = ((resolution * j) + i);


				m_climateMap[index].temperature = (InverseLerp(m_minTemp, m_maxTemp, m_climateMap[index].temperature));
				float temperature = m_climateMap[index].temperature;
				m_climateMap[index].rainfall = (InverseLerp(m_minRainfall, m_maxRainfall, m_climateMap[index].rainfall));
				float rainfall = m_climateMap[index].rainfall;
				m_climateMap[index].climateClassification = m_biomeClassifier.CalculateDistanceToAllBiomes(temperature, rainfall);

				int rValue = m_climateMap[index].climateClassification.x * 255;
				int bValue = m_climateMap[index].climateClassification.y * 255;
				int gValue = m_climateMap[index].climateClassification.z * 255;
			
				m_colourBuffers.at(index) = RGB_TO_UNSIGNED_INT_COLOUR(rValue, bValue, gValue);

			}

		}

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = resolution;
		textureDesc.Height = resolution;
		textureDesc.MipLevels = 1;
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
		size_t rowPitch = (resolution * bpp + 7) / 8;
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
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ClimateMap::GenerateNoiseTexture(ID3D11Device* device)
{
	//int width = m_tempGridWidth;
	int resolution = 128;
	std::vector<uint32_t> m_colourBuffers(resolution * resolution);

	int index = 0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texSRV;
	{

		for (int i = 0; i < resolution; i++) {
			for (int j = 0;j < resolution; j++)
			{
				index = ((resolution * j) + i);


				float tempPerlinVal = (float)m_perlinNoise.Noise((i * 0.05), (j * 0.05), 1);
				int bwValue = abs(tempPerlinVal * 255);
				m_colourBuffers.at(index) = RGB_TO_UNSIGNED_INT_COLOUR(bwValue, 0, 0);

			}

		}

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = resolution;
		textureDesc.Height = resolution;
		textureDesc.MipLevels = 1;
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
		size_t rowPitch = (resolution * bpp + 7) / 8;
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
uint32_t ClimateMap::RGB_TO_UNSIGNED_INT_COLOUR(int r, int g, int b)
{
	return  r | (g << 8) | (b << 16) | (ALPHA << 24);
}

float ClimateMap::InverseLerp(float u, float v, float value)
{

	float inverseLerp = (value - u) / (v - u);
	if (inverseLerp < 0) return 0;
	if (inverseLerp > 1) return 1;
	return inverseLerp;   //Compensates for values which end up outside the range as the amplitude is ignored

}
void ClimateMap::AssessMaxAndMinNoiseValues(float noiseVal, float* maxNoise, float* minNoise)
{
	if (noiseVal > * maxNoise) {
		*maxNoise = noiseVal;
		return;
	}
	if (noiseVal < *minNoise) {
		*minNoise = noiseVal;
	}
}
float* ClimateMap::GetTemperatureAmplitude()
{
	return &m_temperatureAmplitude;
}
float* ClimateMap::GetTemperatureFrequency()
{
	return &m_temperatureFrequency;
}
float* ClimateMap::GetTemperatureOffset()
{
	return &m_temperatureOffset;
}
float* ClimateMap::GetRainfallAmplitude()
{
	return &m_rainfallAmplitude;
}
float* ClimateMap::GetRainfallFrequency()
{
	return &m_rainFallFrequency;
}
float* ClimateMap::GetRainfallOffset()
{
	return &m_rainfallOffset;
}

