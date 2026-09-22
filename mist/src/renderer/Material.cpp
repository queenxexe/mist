#include "renderer/Material.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "renderer/vulkan/VulkanMaterial.hpp"

namespace mist {
	MaterialRef::MaterialRef(const uint32_t materialID) : materialID(materialID) {}

	Material::Material(const uint32_t materialID, const Ref<Shader>& shader) : id(materialID), shader(shader) {}

	MaterialLibrary::MaterialLibrary() : idCounter(0) {}

    Ref<Material> MaterialLibrary::Create(const Ref<Shader>& shader) {
		uint32_t id = idCounter;
		idCounter++;

		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			return nullptr;
		case RenderAPI::API::Vulkan:
			materials[id] = CreateRef<VulkanMaterial>(id, shader);
			return materials[id];
		default:
			MIST_ASSERT(false, "Unknown render API");
			return nullptr;
		}
	}
}