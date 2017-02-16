#include "colorshader.h"

ColorShader::ColorShader() :
	m_vertexShader { 0 },
	m_pixelShader { 0 },
	m_layout { 0 },
	m_matrixBuffer { 0 }
{
}

ColorShader::ColorShader( const ColorShader& other )
{
}

ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize( ID3D11Device* device, HWND hwnd )
{
	bool result { true };

	result = InitializeShader( device, hwnd, L"./shader/color.vs.hlsl", L"./shader/color.ps.hlsl" );
	if ( !result ) { return false; }

	return result;
}

void ColorShader::Shutdown()
{
	ShutdownShader();
}

bool ColorShader::Render( ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix )
{
	bool result { true };

	result = SetShaderParameters( deviceContext, worldMatrix, viewMatrix, projectionMatrix );
	if ( !result ) { return false; }

	RenderShader( deviceContext, indexCount );

	return result;
}

bool ColorShader::InitializeShader( ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename )
{
	HRESULT result { true };
	ID3D10Blob* errorMessage { 0 };
	ID3D10Blob* vertexShaderBuffer { 0 };
	ID3D10Blob* pixelShaderBuffer { 0 };

	////////////////////
	// Set up shaders
	////////////////////

	// Compile the vertex shader file
	result = D3DCompileFromFile( 
		vsFilename, 
		NULL, 
		NULL, 
		"main", 
		"vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 
		0, 
		&vertexShaderBuffer,
		&errorMessage 
	);
	if ( FAILED( result ) )
	{
		if ( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, vsFilename );
		}
		else
		{
			MessageBox( hwnd, vsFilename, L"Missing Shader File", MB_OK );
		}

		return false;
	}

	//Compile the pixle shader file
	result = D3DCompileFromFile(
		psFilename,
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pixelShaderBuffer,
		&errorMessage
	);
	if ( FAILED( result ) )
	{
		if ( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, psFilename );
		}
		else
		{
			MessageBox( hwnd, psFilename, L"Missing Shader File", MB_OK );
		}

		return false;
	}

	// Create the vertex shader
	result = device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader );
	if ( FAILED( result ) ) { return false; }

	// Create the pixel shader
	result = device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader );
	if ( FAILED( result ) ) { return false; }

	////////////////////////////////
	// Set up vertex input layout
	////////////////////////////////

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2] {};
	unsigned int numElements { 0 };

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the Model class and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Create the vertex input layout
	numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[0] );
	result = device->CreateInputLayout(
		polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&m_layout
	);
	if ( FAILED( result ) ) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/////////////////////////////////////////////////
	// Set up the dynamic matrix constant buffer
	/////////////////////////////////////////////////

	D3D11_BUFFER_DESC matrixBufferDesc {};

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer( &matrixBufferDesc, NULL, &m_matrixBuffer );
	if ( FAILED( result ) ) { return false; }

	return true;
}

void ColorShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	if ( m_matrixBuffer )
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if ( m_layout )
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if ( m_pixelShader )
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if ( m_vertexShader )
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

bool ColorShader::SetShaderParameters( ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix )
{
	HRESULT result { true };
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//////////////////////////////////////////
	// Set up global variables in the shader
	//////////////////////////////////////////

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose( worldMatrix );
	viewMatrix = XMMatrixTranspose( viewMatrix );
	projectionMatrix = XMMatrixTranspose( projectionMatrix );

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map( m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if ( FAILED( result ) ) { return false; }

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap( m_matrixBuffer, 0 );

	// Set the updated matrix buffer in the HLSL vertex shader
	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers( bufferNumber, 1, &m_matrixBuffer );

	return true;
}

void ColorShader::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount )
{
	deviceContext->IASetInputLayout( m_layout );

	deviceContext->VSSetShader( m_vertexShader, NULL, 0 );
	deviceContext->PSSetShader( m_pixelShader, NULL, 0 );

	deviceContext->DrawIndexed( indexCount, 0, 0 );
}

void ColorShader::OutputShaderErrorMessage( ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename )
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)( errorMessage->GetBufferPointer() );

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open( "shader-error.txt" );

	// Write out the error message.
	for ( i = 0; i<bufferSize; i++ )
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox( hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK );
}