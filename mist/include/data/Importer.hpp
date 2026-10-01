#pragma once
#include <memory>
#include <string>
#include "data/Mesh.hpp"
#include "renderer/RenderTypes.hpp"

namespace mist {
    class Importer {
    public:
        static std::vector<std::shared_ptr<Mesh>> ImportMeshes(const std::string& path, bool flipWinding = false);
        static ImageData ImportImage(const std::string& path);
    };
}