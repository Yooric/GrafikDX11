#include "TextureShaderClass.h"



TextureShaderClass::TextureShaderClass()
{
	m_VertexShader = 0;
	m_PixelShader = 0;
	m_Layout = 0;
	m_MatrixBuffer = 0;
	m_SampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass &)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* p_Device, HWND p_hwnd)
{
	bool t_Result;
	//initta pixel o vertex cheddarn
	t_Result = InitializeShader(p_Device, p_hwnd, L"texture.vs", L"texture.ps");
	if (!t_Result)
	{
		return false;
	}
	return true;
}

void TextureShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* p_DeviceContext, int p_IndexCount, XMMATRIX p_WorldMatrix, XMMATRIX p_ViewMatrix,
	XMMATRIX p_ProjectionMatrix, ID3D11ShaderResourceView* p_Texture)
{
	bool t_Result;
	t_Result = SetShaderParameters(p_DeviceContext, p_WorldMatrix, p_ViewMatrix, p_ProjectionMatrix, p_Texture);
	if (!t_Result)
	{
		return false;
	}

	//rendrer the prepered buffer med shadern
	RenderShader(p_DeviceContext, p_IndexCount);
	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* p_Device, HWND p_Hwnd, WCHAR* p_VSFilename, WCHAR* p_PSFilename)
{
	HRESULT t_Result;
	ID3D10Blob* t_ErrorMessage;
	ID3D10Blob* t_VertexShaderBuffer;
	ID3D10Blob* t_PixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC t_PolygonLayout[2];
	unsigned int t_NumElements;
	D3D11_BUFFER_DESC t_MatrixBufferDesc;
	
	D3D11_SAMPLER_DESC t_SamplerDesc;
	// Initialize the pointers this function will use to null.
	t_ErrorMessage = 0;
	t_VertexShaderBuffer = 0;
	t_PixelShaderBuffer = 0;

	//kompilera vertex cheddar kod
	t_Result = D3DCompileFromFile(p_VSFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &t_VertexShaderBuffer, &t_ErrorMessage);
	if (FAILED(t_Result))
	{
		if (t_ErrorMessage)
		{
			OutputShaderErrorMessage(t_ErrorMessage, p_Hwnd, p_VSFilename);
		}
		else
		{
			MessageBox(p_Hwnd, p_VSFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}
	//om det inte var något fel är det tydligen att den inte hittar den


	//nu kompilera pixelshader
	t_Result = D3DCompileFromFile(p_PSFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &t_PixelShaderBuffer, &t_ErrorMessage);
	if (FAILED(t_Result))
	{
		if (t_ErrorMessage)
		{
			OutputShaderErrorMessage(t_ErrorMessage, p_Hwnd, p_PSFilename);
		}
		else
		{
			MessageBox(p_Hwnd, p_PSFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	//skapa vertex shader från buffer
	t_Result = p_Device->CreateVertexShader(t_VertexShaderBuffer->GetBufferPointer(), t_VertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
	if (FAILED(t_Result))
	{
		return false;
	}
	//samma fast pixel
	t_Result = p_Device->CreatePixelShader(t_PixelShaderBuffer->GetBufferPointer(), t_PixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
	if (FAILED(t_Result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	t_PolygonLayout[0].SemanticName = "POSITION";
	t_PolygonLayout[0].SemanticIndex = 0;
	t_PolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	t_PolygonLayout[0].InputSlot = 0;
	t_PolygonLayout[0].AlignedByteOffset = 0;
	t_PolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	t_PolygonLayout[0].InstanceDataStepRate = 0;
	
	t_PolygonLayout[1].SemanticName = "TEXCOORD";
	t_PolygonLayout[1].SemanticIndex = 0;
	t_PolygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	t_PolygonLayout[1].InputSlot = 0;
	t_PolygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	t_PolygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	t_PolygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	t_NumElements = sizeof(t_PolygonLayout) / sizeof(t_PolygonLayout[0]);

	// Create the vertex input layout.
	t_Result = p_Device->CreateInputLayout(t_PolygonLayout, t_NumElements, t_VertexShaderBuffer->GetBufferPointer(),
		t_VertexShaderBuffer->GetBufferSize(), &m_Layout);
	if (FAILED(t_Result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	t_VertexShaderBuffer->Release();
	t_VertexShaderBuffer = 0;

	t_PixelShaderBuffer->Release();
	t_PixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	t_MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	t_MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	t_MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	t_MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	t_MatrixBufferDesc.MiscFlags = 0;
	t_MatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	t_Result = p_Device->CreateBuffer(&t_MatrixBufferDesc, NULL, &m_MatrixBuffer);
	if (FAILED(t_Result))
	{
		return false;
	}




	// Create a texture sampler state description.
	t_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	t_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	t_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	t_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	t_SamplerDesc.MipLODBias = 0.0f;
	t_SamplerDesc.MaxAnisotropy = 1;
	t_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	t_SamplerDesc.BorderColor[0] = 0;
	t_SamplerDesc.BorderColor[1] = 0;
	t_SamplerDesc.BorderColor[2] = 0;
	t_SamplerDesc.BorderColor[3] = 0;
	t_SamplerDesc.MinLOD = 0;
	t_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	t_Result = p_Device->CreateSamplerState(&t_SamplerDesc, &m_SampleState);
	if (FAILED(t_Result))
	{
		return false;
	}


	return true;
}

void TextureShaderClass::ShutdownShader()
{
	if (m_SampleState)
	{
		m_SampleState->Release();
		m_SampleState = 0;
	}
	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = 0;
	}
	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = 0;
	}
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = 0;
	}
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = 0;
	}
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* p_ErrorMessage, HWND p_Hwnd, WCHAR* p_ShaderFilename)
{
	char* t_CompileErrors;
	unsigned long long t_BufferSize, i;
	ofstream t_Fout;


	// Get a pointer to the error message text buffer.
	t_CompileErrors = (char*)(p_ErrorMessage->GetBufferPointer());

	// Get the length of the message.
	t_BufferSize = p_ErrorMessage->GetBufferSize();

	// Open a file to write the error message to.
	t_Fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<t_BufferSize; i++)
	{
		t_Fout << t_CompileErrors[i];
	}

	// Close the file.
	t_Fout.close();

	// Release the error message.
	p_ErrorMessage->Release();
	p_ErrorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(p_Hwnd, L"Error compiling shader.  Check shader-error.txt for message.", p_ShaderFilename, MB_OK);

}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* p_DeviceContext, XMMATRIX p_WorldMatrix, XMMATRIX p_ViewMatrix,
	XMMATRIX p_ProjectionMatrix, ID3D11ShaderResourceView* p_Texture)
{
	HRESULT t_Result;
	D3D11_MAPPED_SUBRESOURCE t_MappedResource;
	MatrixBufferType* t_DataPtr;
	unsigned int t_BufferNumber;

	//Transposa matriserna för cheddarn
	p_WorldMatrix = XMMatrixTranspose(p_WorldMatrix);
	p_ViewMatrix = XMMatrixTranspose(p_ViewMatrix);
	p_ProjectionMatrix = XMMatrixTranspose(p_ProjectionMatrix);

	t_Result = p_DeviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &t_MappedResource);
	if (FAILED(t_Result))
	{
		return false;
	}

	//PTR till constantbufferN^
	t_DataPtr = (MatrixBufferType*)t_MappedResource.pData;
	t_DataPtr->world = p_WorldMatrix;
	t_DataPtr->view = p_ViewMatrix;
	t_DataPtr->projection = p_ProjectionMatrix;

	p_DeviceContext->Unmap(m_MatrixBuffer, 0);
	t_BufferNumber = 0;

	p_DeviceContext->VSSetConstantBuffers(t_BufferNumber, 1, &m_MatrixBuffer);
	p_DeviceContext->PSSetShaderResources(0, 1, &p_Texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* p_DeviceContext, int p_IndexCount)
{
	// Set the vertex input layout.
	p_DeviceContext->IASetInputLayout(m_Layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	p_DeviceContext->VSSetShader(m_VertexShader, NULL, 0);
	p_DeviceContext->PSSetShader(m_PixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	p_DeviceContext->PSSetSamplers(0, 1, &m_SampleState);

	// Render the triangle.
	p_DeviceContext->DrawIndexed(p_IndexCount, 0, 0);
}
