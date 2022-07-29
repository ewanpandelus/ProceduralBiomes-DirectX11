#include "pch.h"
#include "Terrain.h"



Terrain::Terrain()
{
	m_terrainGeneratedToggle = false;
}


Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device* device, int terrainWidth, int terrainHeight, int scaleIncrease)
{

	perlinNoise.Initialize();
	int index;
	float height = 0.0;
	bool result;

	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	m_frequency = .2f;
	m_amplitude = 0.5;
	m_lacunarity = 0.5;
	m_octaves = 3;
	m_offset = 0.01;
	m_persistance = 1.25f;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	//this is how we calculate the texture coordinates first calculate the step size there will be between vertices. 
	float textureCoordinatesStep = 5.0f / m_terrainWidth;  //tile 5 times across the terrain. 
	// Initialise the data in the height map (flat).
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i * scaleIncrease;
			m_heightMap[index].y = (float)0;
			m_heightMap[index].z = (float)j * scaleIncrease;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * textureCoordinatesStep;
			m_heightMap[index].v = (float)j * textureCoordinatesStep;

		}
	}

	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}


	return true;
}

void Terrain::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GSSetShader(NULL, NULL, 0);
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);



	return;
}

bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	DirectX::SimpleMath::Vector3* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new DirectX::SimpleMath::Vector3[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index, i, j;
	int index1, index2, index3, index4; //geometric indices. 

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;
	int counter = 0;

	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{

			if (counter % 2 == 0) {

				index1 = (m_terrainHeight * j) + i;             //Bottom left
				index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right
				index3 = (m_terrainHeight * (j + 1)) + i;       //Upper left 
				index4 = (m_terrainHeight * (j + 1)) + (i + 1); //Upper right 
			}
			else {
				index1 = (m_terrainHeight * (j + 1)) + i;       //Upper left 
				index3 = (m_terrainHeight * (j + 1)) + (i + 1); //Upper right 
				index2 = (m_terrainHeight * j) + i;             //Bottom left
				index4 = (m_terrainHeight * j) + (i + 1);      // Bottom right

			}
			counter++;

			SetupVertex(indices, vertices, &index, index3, index4, index1);

			// Upper right.
			SetupVertex(indices, vertices, &index, index4, index3, index1);

			// Bottom left.
			SetupVertex(indices, vertices, &index, index1, index3, index4);

			// Bottom left.
			SetupVertex(indices, vertices, &index, index1, index4, index2);

			// Upper right.
			SetupVertex(indices, vertices, &index, index4, index1, index2);

			// Bottom right.
			SetupVertex(indices, vertices, &index, index2, index1, index4);

		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}
void Terrain::SetupVertex(unsigned long* indices, VertexType* vertices, int* currentIndex, int triangle1Index, int triangle2Index, int triangle3Index)
{
	int index = *currentIndex;
	vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[triangle1Index].x, m_heightMap[triangle1Index].y, m_heightMap[triangle1Index].z);
	vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[triangle1Index].nx, m_heightMap[triangle1Index].ny, m_heightMap[triangle1Index].nz);
	vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[triangle1Index].u, m_heightMap[triangle1Index].v);
	indices[index] = index;
	m_heightMap[triangle1Index].triPos1 = SimpleMath::Vector3(m_heightMap[triangle2Index].x, m_heightMap[triangle2Index].y, m_heightMap[triangle2Index].z);
	m_heightMap[triangle1Index].triPos2 = SimpleMath::Vector3(m_heightMap[triangle3Index].x, m_heightMap[triangle3Index].y, m_heightMap[triangle3Index].z);
	*currentIndex += 1;
}
float Terrain::EvaluateNoiseBasedOnClimate(int index, float xOffset, float zOffset, int i, int j)
{
	SimpleMath::Vector3 climate = climateMap[index].climateClassification;
	float amplitude = (climate.x * 0.3f) + (climate.y) + (climate.z * 2);
	m_amplitude *= amplitude;
	float perlinValue = 0;

	perlinValue =  (1 - abs(perlinNoise.Noise((i+xOffset)  * m_frequency, (j+zOffset)  * m_frequency, 1)) - 0.5)*m_amplitude;



	if (perlinValue < 0) perlinValue = 0;
	return perlinValue;
	
}


void Terrain::SetClimateMap(ClimateMap::ClimateMapType* climateMap)
{
	this->climateMap = climateMap;
}

void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Terrain::GenerateHeightMap(ID3D11Device* device, int scaleIncrease, int xOffset, int zOffset)
{
	Shutdown();
	bool result;
	float maxDepth = 1000;
	int index;


	float initialAmp = m_amplitude;
	float initialFrequency = m_frequency;
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i * scaleIncrease;

			m_heightMap[index].z = (float)j * scaleIncrease;
			float noiseHeight = 0;

			m_amplitude = initialAmp;
			m_frequency = initialFrequency;
			for (int octave = 0; octave < m_octaves; octave++) {
				float perlinValue = EvaluateNoiseBasedOnClimate(index, xOffset, zOffset, i, j);//(float)perlinNoise.Noise((i + xOffset) * m_frequency, (j + zOffset) * m_frequency, 1);
				noiseHeight += perlinValue;
				m_amplitude *= m_persistance;
				m_frequency *= m_lacunarity;
			}
			m_heightMap[index].y = noiseHeight;
			if (noiseHeight < 0 && noiseHeight < maxDepth) {
				maxDepth = noiseHeight;
			}

		}
	}
	m_amplitude = initialAmp;
	m_frequency = initialFrequency;
	m_maxDepth = maxDepth;
	result = true;
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

}
float Terrain::Redistribution(float x, float y, float exponent) {
	float e0 = 1 * perlinNoise.Noise(x * 0.1f, y * 0.1f, 1);
	float e1 = 0.5 * perlinNoise.Noise(2 * x * 0.1f, 2 * y * 0.1f, 1) * e0;
	float e2 = 0.25 * perlinNoise.Noise(4 * x * 0.1f, 4 * y * 0.1f, 1) * (e0 + e1);
	float e = (e0 + e1 + e2) / (1 + 0.5 + 0.25);
	return round(e * exponent) / exponent;
}

bool Terrain::Update()
{
	return true;
}

float Terrain::GetMaxDepth() {
	return m_maxDepth;
}
float* Terrain::GetAmplitude()
{
	return &m_amplitude;
}
Terrain::HeightMapType* Terrain::GetHeightMap()
{
	return m_heightMap;
}
float* Terrain::GetFrequency()
{
	return &m_frequency;
}
int* Terrain::GetOctaves()
{
	return &m_octaves;

}
float* Terrain::GetLacunarity()
{
	return &m_lacunarity;

}
float* Terrain::GetOffset()
{

	return &m_offset;

}

float* Terrain::GetPersistance()
{
	return &m_persistance;
}

