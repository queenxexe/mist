#include "Utils.hpp"
#include <fstream>
#include <filesystem>
#include "Debug.hpp"

namespace mist {
	std::string Utils::ReadFile(const std::string& path) {
		std::string result;
		std::ifstream in(path);

		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		} else {
			MIST_ERROR("Failed to open file at: {0}", path);
		}

		return result;
	}

	bool Utils::Exists(const std::string& path) {
		return std::filesystem::exists(path);
	}
}