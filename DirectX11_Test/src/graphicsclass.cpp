#include "graphicsclass.h"

GraphicsClass::GraphicsClass() :
	m_Direct3D { 0 },
	m_camera { 0 },
	m_model { 0 },
	m_colorShader { 0 }
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
	bool result { true };

	m_Direct3D = new D3DClass();
	if ( !m_Direct3D ) { return false; }

	result = m_Direct3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );
	if ( !result )
	{
		MessageBox( hwnd, L"Could not initialize Direct3D", L"Error", MB_OK );
		return false;
	}

	// Create the camera object.
	m_camera = new Camera;
	if ( !m_camera ) { return false; }

	// Set the initial position of the camera.
	m_camera->SetPosition( 0.0f, 0.0f, -5.0f );

	// Create the model object.
	m_model = new Model;
	if ( !m_model ) { return false; }

	// Initialize the model object.
	result = m_model->Initialize( m_Direct3D->GetDevice() );
	if ( !result )
	{
		MessageBox( hwnd, L"Could not initialize the model object.", L"Error", MB_OK );
		return false;
	}

	// Create the color shader object.
	m_colorShader = new ColorShader;
	if ( !m_colorShader ) { return false; }

	// Initialize the color shader object.
	result = m_colorShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if ( !result )
	{
		MessageBox( hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK );
		return false;
	}

	return result;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if ( m_colorShader )
	{
		m_colorShader->Shutdown();
		delete m_colorShader;
		m_colorShader = 0;
	}

	// Release the model object.
	if ( m_model )
	{
		m_model->Shutdown();
		delete m_model;
		m_model = 0;
	}

	// Release the camera object.
	if ( m_camera )
	{
		delete m_camera;
		m_camera = 0;
	}

	if ( m_Direct3D )
	{
		m_Direct3D->Shutdown();
		delete( m_Direct3D );
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	bool result { true };

	result = Render();
	if ( !result ) { return false; }

	return result;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result { true };

	// Clear the back buffer
	m_Direct3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix
	m_camera->Render();

	// Get matrices.
	m_Direct3D->GetWorldMatrix( worldMatrix );
	m_camera->GetViewMatrix( viewMatrix );
	m_Direct3D->GetProjectionMatrix( projectionMatrix );

	// Put the model vertex and index buffers on the graphics pipeline.
	m_model->Render( m_Direct3D->GetDeviceContext() );

	// Render the model using the shaders
	result = m_colorShader->Render( m_Direct3D->GetDeviceContext(), m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix );
	if ( !result ) { return false; }

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return result;
}