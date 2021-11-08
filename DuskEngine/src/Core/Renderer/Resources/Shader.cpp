#include "pch.h"
#include "Shader.h"

#include "Core/Renderer/RendererContext.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace DuskEngine
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RendererContext::GetAPI())
		{
			case RendererContext::API::None:    return nullptr;
			case RendererContext::API::OpenGL:  return MakeRef<OpenGLShader>(filepath);
			case RendererContext::API::D3D11:	return nullptr;
			default:							return nullptr;
		}
	}
}