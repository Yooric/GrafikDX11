#include "ColorShaderClass.h"





ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass &)
{
}


ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* p_Device, HWND p_hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(p_Device, p_hwnd, L"../EricGrafik2/color.vs", L"../EricGrafik2/color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutDown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}
////// 4123  Här har jag bytt inparametrar till XMFLOAT4X4 från XMMATRIX Då betyder det att man måste converta lite då och då
///för att directx ska bli glad
bool ColorShaderClass::Render(ID3D11DeviceContext* p_DeviceContext, int p_IndexCount,
	XMFLOAT4X4 p_WorldMatrix, XMFLOAT4X4 p_ViewMatrix,
	XMFLOAT4X4 p_ProjectionMatrix)
{
	bool result;
	//Här måste man converta XMFLOAT4X4 till en XMMATRIX nej det behövs inte för nästa funktion bytte jag också grejer!!!
	//MEN! vad var det..... fan.. glömde....
	result = SetShaderParameters(p_DeviceContext,p_WorldMatrix, p_ViewMatrix, p_ProjectionMatrix);
	if (!result)
	{
		return false;
	}
	RenderShader(p_DeviceContext, p_IndexCount);
	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* p_Device, HWND p_hwnd, WCHAR* p_VSFileName, WCHAR* p_PSFilename)
{
	HRESULT result;
	ID3D10Blob* t_ErrorMessage;
	ID3D10Blob* t_VertexShaderBuffer;
	ID3D10Blob* t_PixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC t_PolygonLayout[2];
	unsigned int t_NumElements;
	D3D11_BUFFER_DESC t_MatrixBufferDesc;

	// Initialize the pointers this function will use to null.
	t_ErrorMessage = 0;
	t_VertexShaderBuffer = 0;
	t_PixelShaderBuffer = 0;

	//compajlar vertex shader kåd
	result = D3DCompileFromFile(p_VSFileName, NULL, NULL, "ColorVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &t_VertexShaderBuffer, &t_ErrorMessage);
	if (FAILED(result))
	{
		//Omshadern fackar ska errormessage få något
		if (t_ErrorMessage)
		{
			OutputShaderErrorMessage(t_ErrorMessage, p_hwnd, p_VSFileName);
		}
		////Om den inte har något i errormessage så hittar den inte shadern tydligen
		else
		{
			MessageBox(p_hwnd, p_VSFileName, L"Missing Shader File For Vertex", MB_OK);
		}
		return false;
	}

	//Compajla pixelshaderkåd
	result = D3DCompileFromFile(p_PSFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &t_PixelShaderBuffer, &t_ErrorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (t_ErrorMessage)
		{
			OutputShaderErrorMessage(t_ErrorMessage, p_hwnd, p_PSFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(p_hwnd, p_PSFilename, L"Missing Shader File For Pixel", MB_OK);
		}

		return false;
	}

	//Nu jevlar skapa vertexshader från buffern
	result = p_Device->CreateVertexShader(t_VertexShaderBuffer->GetBufferPointer(), t_VertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}
	//skapa pixelshader från buffern
	result = p_Device->CreatePixelShader(t_PixelShaderBuffer->GetBufferPointer(), t_PixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//skapa vertex input layot description
	//ska stämma överens med VertexType structen i ModelClass i shadern(har inte gjort modelclass men vi får se)
	t_PolygonLayout[0].SemanticName = "POSITION";
	t_PolygonLayout[0].SemanticIndex = 0;
	t_PolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	t_PolygonLayout[0].InputSlot = 0;
	t_PolygonLayout[0].AlignedByteOffset = 0;
	t_PolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	t_PolygonLayout[0].InstanceDataStepRate = 0;
	
	t_PolygonLayout[1].SemanticName = "COLOR";
	t_PolygonLayout[1].SemanticIndex = 0;
	t_PolygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	t_PolygonLayout[1].InputSlot = 0;
	t_PolygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	t_PolygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	t_PolygonLayout[1].InstanceDataStepRate = 0;

	//räkna the elements i layouten
	t_NumElements = sizeof(t_PolygonLayout) / sizeof(t_PolygonLayout[0]);
	//skapa vertex input layout
	result = p_Device->CreateInputLayout(t_PolygonLayout, t_NumElements,
		t_VertexShaderBuffer->GetBufferPointer(), t_VertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}
	//realease verterxshaderbuffern o pixelshaderbuffern eftersom de inte längre behövs

	t_VertexShaderBuffer->Release();
	t_VertexShaderBuffer = 0;
	t_PixelShaderBuffer->Release();
	t_PixelShaderBuffer = 0;


	//Ehhh Setup the description of the dynamic matrix constant buffer that is in the vertex shader. kommer nog mer om detta senare
	t_MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	t_MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	t_MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	t_MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	t_MatrixBufferDesc.MiscFlags = 0;
	t_MatrixBufferDesc.StructureByteStride = 0;


//Skapa constantbuffern pointer so vi kommer åt vertexshaderconstant buffer från denna klassen
	result = p_Device->CreateBuffer(&t_MatrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void ColorShaderClass::ShutdownShader()
{
	//släpper matrix constantbuffern
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//releasar layouten
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//releasra pixelshadern
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	///o vertexshader
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* p_ErrorMessage, HWND p_hwnd, WCHAR* p_ShaderFilename)
{
	char* t_CompileErrors;
	unsigned long long t_BufferSize, i;
	ofstream t_Fout;
	//get pekare till texterrorbuffer
	t_CompileErrors = (char*)(p_ErrorMessage->GetBufferPointer());
	t_BufferSize = p_ErrorMessage->GetBufferSize();
	//skriver ut till textdokument! vafan.. jo men kanske
	t_Fout.open("shader-error.txt");
	for (i = 0; i < t_BufferSize; i++)
	{
		t_Fout << t_CompileErrors[i];
	}
	t_Fout.close();
	p_ErrorMessage->Release();
	p_ErrorMessage = 0;
	//Kan man inte bara dra upp en box med Outputshaderfel... eller är det att det kommer så mycket text kanske=S
	MessageBox(p_hwnd, L"Error compiling shader.  Check shader-error.txt for message.", p_ShaderFilename, MB_OK);
}
//Här byts xmmatrix mot XMFLOAT4X4 och måste sen att convertas tillbaks där skiten används!
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* p_DeviceContext, XMFLOAT4X4 p_World4x4Float, XMFLOAT4X4 p_View4x4Float,
	XMFLOAT4X4 p_Projection4x4Float)
{
	HRESULT t_Result;
	D3D11_MAPPED_SUBRESOURCE t_MappedResource;
	MatrixBufferType* t_DataPtr;
	unsigned int t_BufferNumber;
	XMMATRIX t_WorldMatrix, t_ViewMatrix, t_ProjectionMatrix;
	//Transponsa matrixerna för att göra de redo för shadern men det ska göras under så, aa

	t_WorldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&p_World4x4Float));
	t_ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&p_View4x4Float));
	t_ProjectionMatrix = XMMatrixTranspose(XMLoadFloat4x4(&p_Projection4x4Float));

	//Lås constantbuffern M_matrix så den kan läsas till. (Så man måste väl låsa den innan man kan läsa till den i guess)
	t_Result = p_DeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &t_MappedResource);
	if (FAILED(t_Result))
	{
		return false;
	}
	//Hämtar pointer till constantbuffern
	t_DataPtr = (MatrixBufferType*)t_MappedResource.pData;

	//Kopiera matrixerna till kostantbuffern
	t_DataPtr->world = t_WorldMatrix;
	t_DataPtr->view = t_ViewMatrix;
	t_DataPtr->projection = t_ProjectionMatrix;

	//unlockar constantbuffern
	p_DeviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constantbuffer in the vertex shader
	t_BufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	p_DeviceContext->VSSetConstantBuffers(t_BufferNumber, 1, &m_matrixBuffer);
	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* p_DeviceContext, int p_IndexCount)
{
	//fixar vertex input layout
	p_DeviceContext->IASetInputLayout(m_layout);
	//settar vertexshadern o pixershaders som används för denna triangel!
	p_DeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	p_DeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	//Ut det triangeln=DDDD
	p_DeviceContext->DrawIndexed(p_IndexCount, 0, 0);

}