#pragma once
#include "Core/Macros/DUSK_API.h"

#include "Core/Resources/Resource.h"
#include "Utils/Memory/Memory.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <memory>

namespace DuskEngine
{
	class Shader;
	class Texture;
	enum class UniformType;

	struct Uniform
	{
		std::string Name;
		UniformType Type;
		std::shared_ptr<void> Data = nullptr;

		Uniform() = default;

		Uniform(const std::string& name, UniformType type) : Name(name) , Type(type)
		{}
	};
	
	class DUSK_EXPORT Material : public Resource
	{
	public:
		Material(Ref<Shader>& shader, const std::filesystem::path& path, const uuids::uuid& uuid);
		//Material(Ref<Shader>& shader, const std::string& name = ""); not used anymore, keeping just in case
		//Material(const std::string& shaderPath, const std::string& name = ""); not used anymore, keeping just in case
		~Material();

		void UploadUniforms();
		void UniformsDefaultValue();

		void SetShader(Ref<Shader>& shader);
		
		void SetFloat(const std::string& name, float f);
		void SetVec2(const std::string& name, glm::vec2& v);
		void SetVec3(const std::string& name, glm::vec3& v);
		void SetVec4(const std::string& name, glm::vec4& v);
		void SetTexture(const std::string& name, Ref<Texture>& texture);
		
		const uuids::uuid& GetShaderUUID();
		
		// Serialization
		void SerializeText(const std::string& path);
	private:
		void CreateUniforms();
		Ref<Shader> m_Shader;

		// Map is for direct access to set uniform values
		std::unordered_map<std::string, Uniform*> m_UniformsMap;

		// Vector is where the actual uniform values are located, for iteration - editor drawing and sending to shader
		std::vector<Uniform> m_Uniforms;

		friend class InspectorPanel;
		friend class Scene;
		friend class ResourceManager;
	};
}