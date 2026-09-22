#pragma once
#include <renderer/Shader.hpp>
#include <data/Image.hpp>

namespace mist {
	enum class MaterialParameterType {
		UniformBuffer,
		Texture
	};

	struct MaterialParameter {
		MaterialParameterType type;
		std::vector<std::byte> data;
		Ref<Image> texture;
	};

	// This is designed to be used as a component within the ECS to reference a material
	class MaterialRef {
	public:
		MaterialRef(const uint32_t materialID);

		uint32_t materialID;
	};

	// This is an instance of a materials data and is where data is set to
	class Material {
	public:
		virtual ~Material() {};

		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;
		
		inline const uint32_t& GetID() const { return id; } 
		inline const Ref<Shader>& GetShader() const { return shader; } 
		
		virtual void Cleanup() = 0;
		
		virtual void Bind(const uint8_t renderDataID) = 0;

		virtual void SetTexture(const uint8_t renderDataID, const std::string& name, const Ref<Image>& texture) = 0;
		virtual void SetUniformData(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) = 0;
		virtual void SetPushConstant(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) = 0;
	protected:
		Material(const uint32_t materialID, const Ref<Shader>& shader);

		uint32_t id;
		Ref<Shader> shader;
	};

	// This creates material instances and keeps references of all loaded materials
	class MaterialLibrary {
	public:
		MaterialLibrary();

		Ref<Material> Create(const Ref<Shader>& shader);

		inline Ref<Material> Get(const uint32_t materialID) { return materials[materialID]; }

		const std::unordered_map<uint32_t, Ref<Material>> GetAllMaterials() const { return materials; }
	private:
		std::unordered_map<uint32_t, Ref<Material>> materials;
		uint32_t idCounter;
	};
}