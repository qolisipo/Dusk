#include "pch.h"
#include "Application.h"

#include "Utils/Logging/Logger.h"
#include "Utils/Window/WindowManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "gtc/type_ptr.hpp"

#include "Utils/Logging/Log.h"

namespace DuskEngine
{
	Application::Application()
	{
		Init();
	}

	Application::~Application()
	{
		Shutdown();
	}

	void Application::Init()
	{
		// Initialize subsystems
		Logger::Init();
		WindowManager::Init();
		rendererContext = RendererContext::Create(WindowManager::GetWindow()->GetNativeHandle());
		rendererContext->Init();
		Renderer::Init();
		
		m_Camera = new Camera(glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 100.0f), glm::vec3(-1.0f, 0.0f, 3.0f), glm::vec3(0.0f, -90.0f, 0.0f));
		
		float vertices[] =
		{
			// positions        // texture coords
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,	0.0f, 1.0f  // top left 
		};
		
		unsigned int indices[] =
		{
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		
		m_Shader = std::make_shared<Shader>("res/shaders/simpleTexture.glsl");

		m_Texture = std::make_shared<Texture>("res/textures/uv_mapper.jpg", GL_RGB);

		m_VA.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));

		VertexBufferLayout vbl;
		vbl.Push(ShaderDataType::Float, 3, true);
		vbl.Push(ShaderDataType::Float, 2, true);
		vertexBuffer->SetLayout(vbl);

		m_VA->Bind();
		m_VA->AddBuffer(vertexBuffer);
		m_VA->AddIndices(indexBuffer);
	}

	void Application::Shutdown()
	{
		DUSK_LOG_INFO("##### SHUTDOWN #####");

		// Shutdown subsystems
		Renderer::Shutdown();
		rendererContext->Shutdown();
		WindowManager::Shutdown();
		Logger::Shutdown();
	}

	void Application::Run()
	{
		while (!WindowManager::GetWindow()->ShouldClose())
		{
			RenderCommand::SetClearColor({ 1.0f, 0.0f, 0.0f, 1 });
			RenderCommand::Clear();

			m_Shader->Bind();
			m_Shader->SetUniformMat4("u_ModelViewProjection", m_Camera->GetViewProjectionMatrix());
			m_Texture->Bind(0);
			Renderer::Submit(m_VA);

			rendererContext->SwapBuffers();
		}
	}
}