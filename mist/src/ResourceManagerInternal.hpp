#pragma once
#include <memory>
#include "ResourceManager.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Material.hpp"
#include "data/Image.hpp"

// This is the private interface for accessing data in the resource manager without letting the application
// access it

namespace mist {
    class ResourceManagerInternal {
    public:
        static std::shared_ptr<Material>& GetMaterial(const ResourceManager* manager, const ResourceID id);
        static std::shared_ptr<Shader>& GetShader(const ResourceManager* manager, const ResourceID id);
        static std::shared_ptr<Image>& GetImage(const ResourceManager* manager, const ResourceID id);
    
        static std::unordered_map<ResourceID, std::shared_ptr<Material>> GetAllMaterials(const ResourceManager* manager);

        static void Bind(const ResourceManager* manager, const uint8_t renderDataID, const ShaderRef& shader);
        static void Bind(const ResourceManager* manager, const uint8_t renderDataID, const MaterialRef& material);
    };
}