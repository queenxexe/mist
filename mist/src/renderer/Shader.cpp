#include "renderer/Shader.hpp"
#include "Debug.hpp"
#include "renderer/vulkan/VulkanShader.hpp"
#include "Application.hpp"

namespace mist {
	Ref<Shader> Shader::Create(const std::string& path) {
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::None:
			MIST_ASSERT(false, "No render API set");
			return nullptr;
		case RenderAPI::Vulkan:
			return CreateRef<VulkanShader>(path);
		}

		MIST_ASSERT(false, "Unsupported API selected");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader) {
		auto& name = shader->GetName();
		MIST_ASSERT(!Exists(name), "Shader already exists.");
		shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
		MIST_ASSERT(!Exists(name), "Shader already exists.");
		shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& path) {
		Ref<Shader> shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& path) {
		Ref<Shader> shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name) {
		MIST_ASSERT(Exists(name), "Shader not found");
		return shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const {
		return shaders.find(name) != shaders.end();
	}
}