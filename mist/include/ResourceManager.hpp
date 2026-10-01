#pragma once
#include <string>
#include <memory>
#include "data/RefTypes.hpp"
#include "renderer/RenderTypes.hpp"

namespace mist {
	class ResourceManagerInternal;

	class ResourceManager {
	public:
		ResourceManager();
		~ResourceManager();

		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager& operator=(const ResourceManager& other) = delete;

		void Cleanup();

		MaterialRef CreateMaterial(const ShaderRef& shader);
		ShaderRef CreateShader(const std::string& path);

		// While RGBA8 is good for typical images, use RGBA16F or RGBA32F for skyboxes
		// REPEAT is a good default but for skyboxes use CLAMP
		ImageRef CreateImage(const std::string& path, const TextureFormat desiredFormat = TextureFormat::RGBA8, const TilingMode tiling = TilingMode::REPEAT);

		bool Exists(const MaterialRef& material);
		bool Exists(const ShaderRef& shader);
		bool Exists(const ImageRef& image);

		void SetTexture(const uint8_t renderDataID, const MaterialRef& material, const std::string& name, const ImageRef& image);
	private:
		class Impl;
		std::unique_ptr<Impl> impl;
		friend class ResourceManagerInternal;

		ResourceID NewID();
	};
}