#include "model.h"

Model::Model() :
	m_vertexBuffer { 0 },
	m_indexBuffer { 0 },
	m_vertexCount { 0 },
	m_indexCount { 0 }
{
}

Model::Model( const Model& other )
{
}

Model::~Model()
{
}

bool Model::Initialize( ID3D11Device* device )
{
	bool result { true };

	result = InitializeBuffer( device );
	if ( !result ) { return false; }

	return result;
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::Render( ID3D11DeviceContext* deviceContext )
{
	RenderBuffers( deviceContext );
}

int Model::GetIndexCount()
{
	return m_indexCount;
}

bool Model::InitializeBuffer( ID3D11Device* device )
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	/////////////////////////////////////
	// Fill the vertices and inidices.
	/////////////////////////////////////

	// Set counts as 3 for triangle vertices.
	m_vertexCount = 3;
	m_indexCount = 3;

	vertices = new VertexType[m_vertexCount];
	if ( !vertices ) { return false; }

	indices = new unsigned long[m_indexCount];
	if ( !indices ) { return false; }

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3( -1.0f, -1.0f, 0.0f );  // Bottom left.
	vertices[0].color = XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );

	vertices[1].position = XMFLOAT3( 0.0f, 1.0f, 0.0f );  // Top middle.
	vertices[1].color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

	vertices[2].position = XMFLOAT3( 1.0f, -1.0f, 0.0f );  // Bottom right.
	vertices[2].color = XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	//////////////////////////////////////////
	// Set up the vertex and index buffers.
	//////////////////////////////////////////

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType )*m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer( &vertexBufferDesc, &vertexData, &m_vertexBuffer );
	if ( FAILED( result ) ) { return false; }

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long ) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer( &indexBufferDesc, &indexData, &m_indexBuffer );
	if ( FAILED( result ) ) { return false; }

	delete[] vertices;
	vertices = 0;
	delete[]indices;
	indices = 0;

	return true;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if ( m_indexBuffer )
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if ( m_vertexBuffer )
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void Model::RenderBuffers( ID3D11DeviceContext* deviceContext )
{
	//////////////////////////////////////////////////////////////
	// Set the vertex and index buffers to the graphics pipeline.
	// And set primitive type.
	//////////////////////////////////////////////////////////////

	unsigned int stride { 0 };
	unsigned int offset { 0 };

	stride = sizeof( VertexType );
	offset = 0;

	deviceContext->IASetVertexBuffers( 0, 1, &m_vertexBuffer, &stride, &offset );

	deviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}