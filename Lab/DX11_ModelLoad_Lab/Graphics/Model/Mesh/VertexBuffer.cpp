// Graphics/MeshModel/VertexBuffer.cpp
#include "VertexBuffer.h"

/* defualt */
////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer() :m_stride(0) {}

VertexBuffer::~VertexBuffer() {}


void VertexBuffer::Bind(ID3D11DeviceContext* context)
{
	UINT offset = 0;
	ID3D11Buffer* vbs[] = { m_buffer.Get() };
	context->IASetVertexBuffers(0, 1, vbs, &m_stride, &offset);
} // Bind

