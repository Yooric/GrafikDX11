#include "GraphicsClass.h"



GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
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
	return true;
}

void GraphicsClass::Shutdown()
{
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
	//Clear the buffers to begin scene.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	//visa the rendereed scen till skärmfan
	m_Direct3D->EndScene();

	return true;
}
