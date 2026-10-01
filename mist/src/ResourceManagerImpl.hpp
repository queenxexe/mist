#pragma once
#include <unordered_map>
#include "ResourceManager.hpp"
#include "renderer/Material.hpp"
#include "renderer/Shader.hpp"
#include "data/Image.hpp"

namespace mist {
    class ResourceManager::Impl {
	public:
		uint32_t resouceCounter = 0;
		std::unordered_map<ResourceID, std::shared_ptr<Material>> materials;
		std::unordered_map<ResourceID, std::shared_ptr<Shader>> shaders;
		std::unordered_map<ResourceID, std::shared_ptr<Image>> images;
	};
}