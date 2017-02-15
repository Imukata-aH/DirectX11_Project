#include "graphicsclass.h"

GraphicsClass::GraphicsClass() :
	m_Direct3D { 0 }
{
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result { false };

	m_Direct3D = new D3DClass();
	if ( !m_Direct3D ) { return false; }

	result = m_Direct3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );
	if ( !result )
	{
		MessageBox( hwnd, L"Could not initialize Direct3D", L"Error", MB_OK );
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if ( m_Direct3D )
	{
		m_Direct3D->Shutdown();
		delete( m_Direct3D );
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	bool result { false };

	result = Render();
	if ( !result ) { return false; }

	return true;
}

bool GraphicsClass::Render()
{
	m_Direct3D->BeginScene( 0.5f, 0.5f, 0.5f, 1.0f );
	m_Direct3D->EndScene();

	return true;
}