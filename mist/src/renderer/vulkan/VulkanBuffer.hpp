#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "renderer/Buffer.hpp"

namespace mist {
	class VulkanBuffer {
	public:
		VulkanBuffer();
		~VulkanBuffer();

		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;

		VulkanBuffer(VulkanBuffer&& other) noexcept;
		VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

		void Create(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage allocUsage, VmaAllocationCreateFlags allocFlags, VmaAllocationInfo& info);
		void Copy(VkBuffer src, VkBuffer dst, VkDeviceSize size);
		void SetData(VkDeviceSize size, const void* data, VkBufferUsageFlags usage);
		void Clear();
		void Reset();

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation alloc = VK_NULL_HANDLE;
		VkDeviceSize size = 0;
	};

	class VulkanVertexBuffer : public VertexBuffer {
	public:
		VulkanVertexBuffer(const std::vector<Vertex>& vertices);

		virtual void Clear() override { buffer.Clear(); }
		virtual void Bind() const override;
		virtual void SetData(const std::vector<Vertex>& vertices) override;

		inline const VkBuffer& GetBuffer() const { return buffer.buffer; }
		inline const VkDeviceSize GetSize() const { return buffer.size; }
	private:
		VulkanBuffer buffer;
	};

	class VulkanIndexBuffer : public IndexBuffer {
	public:
		VulkanIndexBuffer(const std::vector<uint32_t>& indices);

		virtual void Clear() override { buffer.Clear(); }
		virtual void Bind() const override;
		virtual void SetData(const std::vector<uint32_t>& indices) override;

		inline const VkBuffer& GetBuffer() const { return buffer.buffer; }
		inline const VkDeviceSize GetSize() const { return buffer.size; }
	private:
		VulkanBuffer buffer;
	};

	class UniformBuffer {
	public:
		UniformBuffer();
		UniformBuffer(const uint32_t size, const void* data);
		~UniformBuffer();

		UniformBuffer(UniformBuffer&& other) noexcept;
		UniformBuffer& operator=(UniformBuffer&& other) noexcept;

		// Returns true if buffer is created/recreated
		bool SetData(const uint32_t size, const void* data);
		void Clear();

		inline const VkBuffer& GetBuffer() const { return buffer.buffer; }
		inline const VkDeviceSize GetSize() const { return buffer.size; }
	private:
		VulkanBuffer buffer;
		void* mappedData = nullptr;
	};
}