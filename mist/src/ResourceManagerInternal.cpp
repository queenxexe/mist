#include "ResourceManagerInternal.hpp"
#include "ResourceManagerImpl.hpp"

namespace mist {
	std::shared_ptr<Material>& ResourceManagerInternal::GetMaterial(const ResourceManager* manager, const ResourceID id) {
		return manager->impl->materials[id];
	}

	std::shared_ptr<Shader>& ResourceManagerInternal::GetShader(const ResourceManager* manager, const ResourceID id) {
		return manager->impl->shaders[id];
	}

	std::shared_ptr<Image>& ResourceManagerInternal::GetImage(const ResourceManager* manager, const ResourceID id) {
		return manager->impl->images[id];
	}

	std::unordered_map<ResourceID, std::shared_ptr<Material>> ResourceManagerInternal::GetAllMaterials(const ResourceManager* manager) {
		return manager->impl->materials;
	}

	void ResourceManagerInternal::Bind(const ResourceManager* manager, const uint8_t renderDataID, const ShaderRef& shader) {
		manager->impl->shaders[shader.id]->Bind(renderDataID);
	}

	void ResourceManagerInternal::Bind(const ResourceManager* manager, const uint8_t renderDataID, const MaterialRef& material) {
		manager->impl->materials[material.id]->Bind(renderDataID);
	}
}