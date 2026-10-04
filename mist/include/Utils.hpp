#pragma once
#include <string>

namespace mist {
	class Utils {
	public:
		static std::string ReadFile(const std::string& path);
		static bool Exists(const std::string& path);
		static std::string GetAbsolutePath(const std::string& path);
		static std::string GetParentPath(const std::string& path);
		static std::string GetFileName(const std::string& filePath);
		static std::string GetFileNameWithoutExtension(const std::string& filePath);
		static std::string GetFileExtension(const std::string& filePath);

		// Gets any default engine shader path e.g. lambert.glsl
		// Example: std::string path = GetEngineShaderPath("lambert");
		static std::string GetEngineShaderPath(const std::string& shaderName);
	};

	class FileDialog {
	public:
		static std::string OpenFile(const std::string& filter);
		static std::string SaveFile(const std::string& filter);
	};
}