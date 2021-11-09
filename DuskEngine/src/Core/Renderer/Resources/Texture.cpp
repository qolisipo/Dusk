#include "pch.h"
#include "Texture.h"

#include "Core/Renderer/RendererContext.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace DuskEngine
{
	Ref<Texture> Texture::Create(const std::string& filepath, const std::string& name)
    {
		switch (RendererContext::GetAPI())
		{
			case RendererContext::API::None:    return nullptr;
			case RendererContext::API::OpenGL:  return MakeRef<OpenGLTexture>(filepath, name);
			case RendererContext::API::D3D11:	return nullptr;
			default:							return nullptr;
		}
    }
}