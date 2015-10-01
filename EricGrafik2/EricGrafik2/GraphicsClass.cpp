#include "GraphicsClass.h"




GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass &)
{
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	XMFLOAT3 t_StartCamera = XMFLOAT3(0.0f, 0.0f, -15.0f);////StartcameraVärden!!

	//Skapar direct3d objektet
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}
	//inita 
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		//MessageBox(hwnd, L"Could not initialze Direct3D tydligen!", L"Error", MB_OK);
		return false;
	}
	//skapa cameraobjektet
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	//sätter startpositionen för Kameran!
	m_Camera->SetPosition(t_StartCamera.x, t_StartCamera.y, t_StartCamera.z);
	//skapar modelklass
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	//initar model
	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object hoppas denna kommer nogon gang", L"Error", MB_OK);
		return false;
	}
	//create the color cheddar object
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}
	//init cheddarn color
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(),hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could NOt initialize the color shader object", L"Error", MB_OK);
	}





	return true;
}

void GraphicsClass::Shutdown()
{
	//realeasra color shader object
	if (m_ColorShader)
	{
		m_ColorShader->ShutDown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	//Releaseaaaaaaaaa model objectet
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// releasa på camera

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}




	//kopplar bort direct3d object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	//renderar grafiken
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX t_WorldMatrix, t_ViewMatrix, t_ProjectionMatrix;
	XMFLOAT4X4 t_World4x4, t_View4x4, t_Projection4x4;

	bool t_Result;
	//Clear the buffers to begin scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	//Generate the viewmatrix based on camerapositionen
	m_Camera->Render();

	//Get world view and proj matriix från camera o d3d objekt
	m_Direct3D->GetWorldMatrix(t_WorldMatrix);
	m_Camera->GetViewMatrix(t_ViewMatrix);
	m_Direct3D->GetProjectionMatrix(t_ProjectionMatrix);

	//Put the model vertex and index buffer på grafikpipelinen o förbered dem för drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//render the model using colorshader!!
	XMStoreFloat4x4(&t_World4x4, t_WorldMatrix);
	XMStoreFloat4x4(&t_View4x4, t_ViewMatrix);
	XMStoreFloat4x4(&t_Projection4x4, t_ProjectionMatrix);

	t_Result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), t_World4x4, t_View4x4, t_Projection4x4);
	if (!t_Result)
	{
		return false;
	}

	//visa the rendereed scen till skärmfan
	m_Direct3D->EndScene();

	return true;
}
