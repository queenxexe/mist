#pragma once
#include <string>
#include "Core.hpp"

namespace mist {
	class Utils {
	public:
		static std::string ReadFile(const std::string& path);
		static bool Exists(const std::string& path);
	};

	class FileDialog {
	public:
		static std::string OpenFile(const std::string& filter);
		static std::string SaveFile(const std::string& filter);
	};
}