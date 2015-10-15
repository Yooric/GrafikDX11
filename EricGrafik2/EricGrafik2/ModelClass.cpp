#include "ModelClass.h"



ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass &)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, char* p_textureFilename)
{
	bool t_Result;
	t_Result = InitializeBuffers(p_device);
	if (!t_Result)
	{
		return false;
	}


	//ladda texturen va
	t_Result = LoadTexture(p_device, p_deviceContext, p_textureFilename);
	if (!t_Result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* p_DeviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(p_DeviceContext);
}

ID3D11ShaderResourceView * ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::LoadTexture(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, char* p_filename)
{
	bool t_result;
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}
	t_result = m_Texture->Initialize(p_device, p_deviceContext, p_filename);
	if (!t_result)
	{
		return false;
	}
	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

bool ModelClass::InitializeBuffers(ID3D11Device* p_Device)
{
	//Usually you would read in a model and create the buffers from that data file.
	//For this tutorial we will just set the points in the vertex and index buffer manually since it is only a single triangle.
	VertexType* t_Vertices;
	unsigned long* t_Indices;
	D3D11_BUFFER_DESC t_VertexBufferDesc, t_IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA t_VertexData, t_IndexData;
	HRESULT t_Result;

	// 3 Är nog för 1 triangel ...... Ja
	m_vertexCount = 4;

	// 3 Är nog för 1 triangel
	m_indexCount = 6;

	// Create the vertex array.
	t_Vertices = new VertexType[m_vertexCount];
	if (!t_Vertices)
	{
		return false;
	}

	// Create the index array.
	t_Indices = new unsigned long[m_indexCount];
	if (!t_Indices)
	{
		return false;
	}

	///om man vill göra en model utan texturer använder man den andra structen VertexTypeWithoutTex
	// Load the vertex array with data.
	//t_Vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//t_Vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//t_Vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top Left.
	//t_Vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//t_Vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	//t_Vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//t_Vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	//t_Vertices[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	
	t_Vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	t_Vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	t_Vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top Left.
	t_Vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	t_Vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	t_Vertices[2].texture = XMFLOAT2(1.0f, 1.0f);



	// Load the index array with data.
	t_Indices[0] = 0;  // Bottom left.
	t_Indices[1] = 1;  // Top Left
	t_Indices[2] = 2;  // Bottom right.
	t_Indices[3] = 1; //TopRight 
	t_Indices[4] = 3;  
	t_Indices[5] = 2;  

	//Description för static vertexbuffer

	t_VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	t_VertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	t_VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	t_VertexBufferDesc.CPUAccessFlags = 0;
	t_VertexBufferDesc.MiscFlags = 0;
	t_VertexBufferDesc.StructureByteStride = 0;

	//Give the SUbresource?? structure a pointer to the vertexdata
	t_VertexData.pSysMem = t_Vertices;
	t_VertexData.SysMemPitch = 0;
	t_VertexData.SysMemSlicePitch = 0;

	//skapa vertexBuffern
	t_Result = p_Device->CreateBuffer(&t_VertexBufferDesc, &t_VertexData, &m_vertexBuffer);
	if (FAILED(t_Result))
	{
		return false;
	}
	//Description of the static index buffer
	t_IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	t_IndexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	t_IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	t_IndexBufferDesc.CPUAccessFlags = 0;
	t_IndexBufferDesc.MiscFlags = 0;
	t_IndexBufferDesc.StructureByteStride = 0;

	// Give the subresource(den här rackarn igen) structure a pointer to the index data.
	t_IndexData.pSysMem = t_Indices;
	t_IndexData.SysMemPitch = 0;
	t_IndexData.SysMemSlicePitch = 0;

	//skapa indexbuffer!
	t_Result = p_Device->CreateBuffer(&t_IndexBufferDesc, &t_IndexData, &m_indexBuffer);
	if (FAILED(t_Result))
	{
		return false;
	}
	//After the vertex buffer and index buffer have been created you can delete 
	//the vertex and index arrays as they are no longer needed since the data was copied into the buffers.
	delete[] t_Vertices;
	t_Vertices = 0;
	delete[] t_Indices;
	t_Indices = 0;
	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* p_DeviceContext)
{
	///// In this tutorial we set the vertex buffer and index buffer as active on the input
	//assembler and tell the GPU that the buffers should be drawn as triangles using the IASetPrimitiveTopology DirectX function.

	unsigned int t_Stride;
	unsigned int t_Offset;
	t_Stride = sizeof(VertexType);
	t_Offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	p_DeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &t_Stride, &t_Offset);
	//samma som över fast index buffer, fast andra inparametrar
	p_DeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	p_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
