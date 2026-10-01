#pragma once
#include <memory>
#include <string>
#include "data/Mesh.hpp"
#include "renderer/Buffer.hpp"
#include "components/Transform.hpp"

namespace mist {
    class MeshRenderer {
    public:
        MeshRenderer(Transform& transform, std::shared_ptr<Mesh> mesh);
        ~MeshRenderer();

        void Bind(const uint8_t renderDataID);
        void Draw();
        void Apply();
        void Clear();

        inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<VertexBuffer> vBuffer;
        std::shared_ptr<IndexBuffer> iBuffer;
    private:
        Transform& transformComponent;
    };
}