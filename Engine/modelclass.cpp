////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelclass.h"

using namespace DirectX;

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

}
ModelClass::~ModelClass()
{
}


bool ModelClass::InitializeModel(ID3D11Device* device, char* filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture)

{
	LoadModel(filename);
	this->m_texture = texture;
	return false;
}



void ModelClass::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	if (m_instanceCount == 0) {
		return;
	}
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	//deviceContext->DrawIndexed(m_indexCount, 0, 0);
	//deviceContext->Draw(m_vertexCount, 0);
	deviceContext->DrawIndexedInstanced(m_indexCount, m_instanceCount, 0, 0, 0);
	return;
}

void ModelClass::IndexRender(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);
	//deviceContext->Draw(m_vertexCount, 0);
	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ModelClass::GetTexture()
{
	return m_texture;
}
void ModelClass::AddNewPosition(DirectX::SimpleMath::Vector3 position) {
	m_positions.push_back(position);
	GenerateRandomScale();
	m_instanceCount++;
}
void ModelClass::GenerateRandomScale() {
	float random = rand() % 21;
	m_scales.push_back((1 + (random /100.f)));
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	if (m_instanceCount == 0) {
		return false;
	}
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

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

	// Load the vertex array and index array with data from the pre-fab
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = DirectX::SimpleMath::Vector3(preFabVertices[i].position.x, preFabVertices[i].position.y, preFabVertices[i].position.z);
		vertices[i].texture = DirectX::SimpleMath::Vector2(preFabVertices[i].textureCoordinate.x, preFabVertices[i].textureCoordinate.y);
		vertices[i].normal = DirectX::SimpleMath::Vector3(preFabVertices[i].normal.x, preFabVertices[i].normal.y, preFabVertices[i].normal.z);
	}
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = preFabIndices[i];
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
	std::vector<InstanceType> instances;
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;

	// Set the number of instances in the array.
	
	UINT rt = cbrt(m_instanceCount);

	InstanceType it;

	// Load the instance array with data.
	for (UINT inst = 0; inst < m_instanceCount; inst++) {
		it.position = m_positions[inst];
		it.scale = m_scales[inst];
		instances.push_back(it);
	}

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = &(instances.front());
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	return true;
}

void ModelClass::ClearInstances()
{
	m_instanceCount = 0;
	m_positions.clear();
	m_scales.clear();
}


void ModelClass::ShutdownBuffers()
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


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT strides[2];
	UINT offsets[2];

	ID3D11Buffer* bufferPointers[2];

	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set vertex buffer stride and offset.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadModel(char* filename)
{
	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> norms;
	std::vector<XMFLOAT2> texCs;
	std::vector<unsigned int> faces;

	FILE* file;// = fopen(filename, "r");
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if (err != 0)
		//if (file == NULL)
	{
		return false;
	}

	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);

			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
				//norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[12];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
					&face[3], &face[4], &face[5],
					&face[6], &face[7], &face[8],
					&face[9], &face[10], &face[11]);

				if (matches == 12) {
					int num[18] = { 0,1,2,3,4,5,6,7,8,0,1,2,6,7,8,9,10,11 };

					for (int i = 0; i < 18; i++) {
						faces.push_back(face[num[i]]);
					}
				}

				else if (matches == 9) {
					/*int num[9] = { 0,1,2,3,4,5,6,7,8};*/
					int num[9] = { 6,7,8, 0,1,2, 3,4,5 };
					for (int i = 0; i < 9; i++)
					{
						faces.push_back(face[num[i]]);
					}

				}
				else {
					return false;
				}
			}
		}
	}

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	//// Create the model using the vertex count that was read in.
	m_vertexCount = numFaces * 3;
	// 
	// m_vertexCount = verts.size();
//	model = new ModelType[vertexCount];

	// "Unroll" the loaded obj information into a list of triangles.
	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		VertexPositionNormalTexture tempVertex;
		tempVertex.position.x = verts[(faces[f + 0] - 1)].x;
		tempVertex.position.y = verts[(faces[f + 0] - 1)].y;
		tempVertex.position.z = verts[(faces[f + 0] - 1)].z - 1;

		tempVertex.textureCoordinate.x = texCs[(faces[f + 1] - 1)].x;
		tempVertex.textureCoordinate.y = 1 - texCs[(faces[f + 1] - 1)].y;
		//tempVertex.textureCoordinate.y = texCs[(faces[f + 1] - 1)].y;

		tempVertex.normal.x = norms[(faces[f + 2] - 1)].x;
		tempVertex.normal.y = norms[(faces[f + 2] - 1)].y;
		tempVertex.normal.z = norms[(faces[f + 2] - 1)].z - 1;

		//increase index count
		preFabVertices.push_back(tempVertex);

		int tempIndex;
		tempIndex = vIndex;
		preFabIndices.push_back(tempIndex);
		vIndex++;
	}
	m_indexCount = vIndex;

	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();
	return true;
}


void ModelClass::ReleaseModel()
{
	return;
}

