#pragma once
#include <string>
#include "data/Mesh.hpp"
#include "renderer/Buffer.hpp"
#include "components/Transform.hpp"

namespace mist {
    class MeshRenderer {
    public:
        MeshRenderer(Transform& transform, Ref<Mesh> mesh);
        ~MeshRenderer();

        void Bind(const uint8_t renderDataID);
        void Draw();
        void Apply();
        void Clear();

        inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

        Ref<Mesh> mesh;
        Ref<VertexBuffer> vBuffer;
        Ref<IndexBuffer> iBuffer;
    private:
        Transform& transformComponent;
    };
}