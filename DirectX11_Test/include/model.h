#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	Model();
	Model( const Model& );
	~Model();

	bool Initialize( ID3D11Device* device );
	void Shutdown();
	void Render( ID3D11DeviceContext* deviceContext );

	int GetIndexCount();

private:
	bool InitializeBuffer( ID3D11Device* device );
	void ShutdownBuffers();
	void RenderBuffers( ID3D11DeviceContext* deviceContext );

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
};