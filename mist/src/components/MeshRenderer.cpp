#include "components/MeshRenderer.hpp"
#include "Application.hpp"

namespace mist {
	MeshRenderer::MeshRenderer(Transform& transform, std::shared_ptr<Mesh> mesh) : transformComponent(transform), mesh(mesh) {
		Apply();
	}

	MeshRenderer::~MeshRenderer() {}

	void MeshRenderer::Bind(const uint8_t renderDataID) {
		Application::Get().GetRenderAPI()->BindMeshRenderer(renderDataID, *this);
	}
	
	void MeshRenderer::Draw() {
		Application::Get().GetRenderAPI()->Draw(mesh->indices.size());
	}

	void MeshRenderer::Apply() {
		Clear();
		vBuffer = mist::VertexBuffer::Create(mesh->vertices);
		iBuffer = mist::IndexBuffer::Create(mesh->indices);
	}

	void MeshRenderer::Clear() {
		if (vBuffer != nullptr)
			vBuffer->Clear();

		if (iBuffer != nullptr)
			iBuffer->Clear();
	}
}