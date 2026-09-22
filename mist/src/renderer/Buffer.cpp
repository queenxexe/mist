#include "renderer/Buffer.hpp"
#include "Debug.hpp"
#include "Application.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"

namespace mist {
	Ref<VertexBuffer> VertexBuffer::Create(const std::vector<Vertex>& vertices) {
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			return nullptr;
		case RenderAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(vertices);
		default:
			MIST_ASSERT(false, "Unknown renderer API");
			return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(const std::vector<uint32_t>& indices) {
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::None:
			MIST_ASSERT(false, "None render API not supported");
			return nullptr;
		case RenderAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(indices);
		default:
			MIST_ASSERT(false, "Unknown renderer API");
			return nullptr;
		}
	}

	BufferLayout::BufferLayout() : elements(nullptr), elementCount(0), capacity(0), size(0), stride(0) {}

	BufferLayout::BufferLayout(BufferElement** elements, uint32_t elementCount) : elements(elements), elementCount(elementCount), capacity(elementCount), size(0), stride(0) {
		CalculateOffsetAndStride();
	}

	void BufferLayout::Push(BufferElement* element) {
		if (size == capacity) {
			Resize(capacity == 0 ? 1 : capacity * 2);
		}
		elements[size++] = element;
		CalculateOffsetAndStride();
	}

	void BufferLayout::Pop(BufferElement* layer) {
		auto it = std::find(begin(), end(), layer);
		if (it != end()) {
			std::rotate(it, it + 1, end());
			--size;
		}
		CalculateOffsetAndStride();
	}

	void BufferLayout::Clear() {
		delete[] elements;
		size = 0;
		stride = 0;
	}

	void BufferLayout::Resize(uint32_t newCapacity) {
		BufferElement** newElements = new BufferElement*[newCapacity];
		std::copy(begin(), end(), newElements);
		delete[] elements;
		elements = newElements;
		capacity = newCapacity;
	}

	void BufferLayout::CalculateOffsetAndStride() {
		uint32_t offset = 0;
		stride = 0;
		for (size_t i = 0; i < Size(); i++) {
			elements[i]->offset = offset;
			offset += Size();
			stride += Size();
		}
	}
}