#include "pch.h"
#include "Mesh.h"

#include "Platform/OpenGL/VertexArray.h"
#include "Platform/OpenGL/Buffer.h"

#include "Platform/OpenGL/OpenGLAPI.h"

namespace DuskEngine
{
	Mesh::Mesh(float* vertices, unsigned int size, unsigned int* indices, unsigned int count, MeshType type)
		:m_Type(type)
	{
		VertexBufferLayout vbl;
		vbl.Push(ShaderDataType::FLOAT, 3, true);
		vbl.Push(ShaderDataType::FLOAT, 2, true);
		vbl.Push(ShaderDataType::FLOAT, 3, true);
		
		m_VA.Bind();

		VertexBuffer vb;
		vb.BufferType = BufferType::ARRAY_BUFFER;
		vb.UsageType = UsageType::STATIC_DRAW;
		vb.Size = size;
		vb.Layout = vbl;

		OpenGLAPI::SetBufferData(vb.ResourceID, vb.BufferType, vb.UsageType, vertices, vb.Size);
		
		IndexBuffer ib;
		ib.BufferType = BufferType::ELEMENT_ARRAY_BUFFER;
		ib.UsageType = UsageType::STATIC_DRAW;
		ib.Size = count * sizeof(unsigned int);
		ib.Count = count;

		OpenGLAPI::SetBufferData(ib.ResourceID, ib.BufferType, ib.UsageType, indices, ib.Size);

		m_VA.SetBuffer(vb);
		m_VA.SetIndices(ib);
	}

	Mesh::Mesh(std::vector<Vertex>& vertices, unsigned int* indices, unsigned int count)
		:m_Type(MeshType::Model)
	{
		VertexBufferLayout vbl;
		vbl.Push(ShaderDataType::FLOAT, 3, true);
		vbl.Push(ShaderDataType::FLOAT, 2, true);
		vbl.Push(ShaderDataType::FLOAT, 3, true);

		m_VA.Bind();

		VertexBuffer vb;
		vb.BufferType = BufferType::ARRAY_BUFFER;
		vb.UsageType = UsageType::STATIC_DRAW;
		vb.Size = vertices.size() * sizeof(Vertex);
		vb.Layout = vbl;

		OpenGLAPI::SetBufferData(vb.ResourceID, vb.BufferType, vb.UsageType, &vertices[0], vb.Size);

		IndexBuffer ib;
		ib.BufferType = BufferType::ELEMENT_ARRAY_BUFFER;
		ib.UsageType = UsageType::STATIC_DRAW;
		ib.Size = count * sizeof(unsigned int);
		ib.Count = count;

		OpenGLAPI::SetBufferData(ib.ResourceID, ib.BufferType, ib.UsageType, indices, ib.Size);
		
		m_VA.SetBuffer(vb);
		m_VA.SetIndices(ib);
	}

	Mesh::~Mesh()
	{
	}
}