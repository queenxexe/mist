#pragma once
#include <string>
#include "Core.hpp"
#include "data/Mesh.hpp"
#include "data/Image.hpp"

namespace mist {
    class Importer {
    public:
        static std::vector<Ref<Mesh>> ImportMeshes(const std::string& path, bool flipWinding = false);
        static ImageData ImportImage(const std::string& path);
    };
}