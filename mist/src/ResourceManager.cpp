#include "ResourceManager.hpp"
#include "ResourceManagerImpl.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "renderer/vulkan/VulkanMaterial.hpp"
#include "renderer/vulkan/VulkanImage.hpp"

namespace mist {
	ResourceManager::ResourceManager() : impl(std::make_unique<Impl>()) {}

	ResourceManager::~ResourceManager() {}

	void ResourceManager::Cleanup() {
		MIST_INFO("Clearing materials");
		for (const auto& [id, material] : impl->materials)
			material->Cleanup();
		impl->materials.clear();
		
		MIST_INFO("Clearing shaders");
		for (const auto& [id, shader] : impl->shaders)
			shader->Cleanup();
		impl->shaders.clear();
		
		MIST_INFO("Clearing images");
		for (const auto& [id, image] : impl->images)
			image->Cleanup();
		impl->images.clear();
	}

	MaterialRef ResourceManager::CreateMaterial(const ShaderRef& shader) {
		MaterialRef ref{};
		ref.id = INVALID_RESOURCE_ID;

		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			break;
		case RenderAPI::API::Vulkan:
			ref.id = NewID();
			impl->materials[ref.id] = std::make_shared<VulkanMaterial>(shader);
			break;
		default:
			MIST_ASSERT(false, "Unknown render API");
		}

		return ref;
	}

	ShaderRef ResourceManager::CreateShader(const std::string& path) {
		ShaderRef ref{};
		ref.id = INVALID_RESOURCE_ID;

		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			break;
		case RenderAPI::API::Vulkan:
			ref.id = NewID();
			impl->shaders[ref.id] = std::make_shared<VulkanShader>(path);
			break;
		default:
			MIST_ASSERT(false, "Unknown render API");
		}

		return ref;
	}

	ImageRef ResourceManager::CreateImage(const std::string& path, const TextureFormat desiredFormat, const TilingMode tiling) {
		ImageRef ref{};
		ref.id = INVALID_RESOURCE_ID;
		
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			break;
		case RenderAPI::API::Vulkan:
			ref.id = NewID();
			impl->images[ref.id] = std::make_shared<VulkanImage>(path, desiredFormat, tiling);
			break;
		default:
			MIST_ASSERT(false, "Unknown render API");
		}

		return ref;
	}

	bool ResourceManager::Exists(const MaterialRef& material) {
		return impl->materials.find(material.id) != impl->materials.end();
	}

	bool ResourceManager::Exists(const ShaderRef& shader) {
		return impl->shaders.find(shader.id) != impl->shaders.end();
	}

	bool ResourceManager::Exists(const ImageRef& image) {
		return impl->images.find(image.id) != impl->images.end();
	}

	void ResourceManager::SetTexture(const uint8_t renderDataID, const MaterialRef& material, const std::string& name, const ImageRef& image) {
		impl->materials[material.id]->SetTexture(renderDataID, name, impl->images[image.id]);
	}

	// Very simple id creation which may have issues later or with larger projects 
	// but will be fine for now
	ResourceID ResourceManager::NewID() {
		ResourceID newID = impl->resouceCounter;
		impl->resouceCounter++;
		return newID;
	}
}