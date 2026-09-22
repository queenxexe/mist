#include "renderer/vulkan/VulkanBuffer.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
	VulkanBuffer::VulkanBuffer() {}

	VulkanBuffer::~VulkanBuffer() {
		Clear();
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept : buffer(other.buffer), alloc(other.alloc), size(other.size) {
		other.Reset();
	}
	
	VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
		if (this != &other) {
			this->Clear();
			this->buffer = other.buffer;
			this->alloc = other.alloc;
			this->size = other.size;
			other.Reset();
		}
		
		return *this;
	}

	void VulkanBuffer::Create(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage allocUsage, VmaAllocationCreateFlags allocFlags, VmaAllocationInfo& info) {
		MIST_ASSERT(buffer == VK_NULL_HANDLE, "Trying to create a Vulkan buffer when one already exists");
		
		VulkanContext& context = VulkanContext::GetContext();

		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo {};
		allocCreateInfo.usage = allocUsage;
		allocCreateInfo.flags = allocFlags;

		CheckVkResult(vmaCreateBuffer(context.GetAllocator(), &bufferInfo, &allocCreateInfo, &buffer, &alloc, &info));
		this->size = size; 
	}

	void VulkanBuffer::Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginSingleTimeCommands();
		
		VkBufferCopy copyRegion {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(context.GetTempCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
		
		context.EndSingleTimeCommands();
	}
	
	void VulkanBuffer::SetData(VkDeviceSize size, const void* data, VkBufferUsageFlags usage) {
		MIST_ASSERT(size != 0, "Trying to set vulkan buffer data with a size of 0");
		MIST_ASSERT(data != nullptr, "Trying to set vulkan buffer data with data that is a nullptr");

		VulkanContext& context = VulkanContext::GetContext();
		if (this->size != size) {
			Clear();
			VmaAllocationInfo info{};
			Create(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VMA_MEMORY_USAGE_GPU_ONLY, 0, info);
		}
		
		VulkanBuffer staging;
		VmaAllocationInfo info {};
		staging.Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, info);
		
		memcpy(info.pMappedData, data, size);
		vmaFlushAllocation(context.GetAllocator(), staging.alloc, 0, size);

		Copy(staging.buffer, buffer, size);
	}

	void VulkanBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();
		
		if (buffer != VK_NULL_HANDLE) {
			vmaDestroyBuffer(context.GetAllocator(), buffer, alloc);
			Reset();
		}
	}

	void VulkanBuffer::Reset() {
		buffer = VK_NULL_HANDLE;
		alloc = VK_NULL_HANDLE;
		size = 0;
	}
	
	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& vertices) {
		SetData(vertices);
	}
	
	void VulkanVertexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(context.GetCurrentFrameCommandBuffer(), 0, 1, &buffer.buffer, offsets);
	}
	
	void VulkanVertexBuffer::SetData(const std::vector<Vertex>& vertices) {
		const VkDeviceSize size = sizeof(Vertex) * vertices.size();
		buffer.SetData(size, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}
	
	VulkanIndexBuffer::VulkanIndexBuffer(const std::vector<uint32_t>& indices) {
		SetData(indices);
	}
	
	void VulkanIndexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdBindIndexBuffer(context.GetCurrentFrameCommandBuffer(), buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	
	void VulkanIndexBuffer::SetData(const std::vector<uint32_t>& indices) {
		const VkDeviceSize size = indices.size() * sizeof(uint32_t);
		buffer.SetData(size, indices.data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}
	
	UniformBuffer::UniformBuffer() {}
	
	UniformBuffer::UniformBuffer(const uint32_t size, const void* data) {
		SetData(size, data);
	}
	
	UniformBuffer::~UniformBuffer() {}
	
	UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept : buffer(std::move(other.buffer)), mappedData(other.mappedData) {
		other.mappedData = nullptr;
	}
	
	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {
		if (this != &other) {
			this->Clear();
			this->buffer = std::move(other.buffer);
			this->mappedData = other.mappedData;
			other.mappedData = nullptr;
		}
		
		return *this;
	}

	bool UniformBuffer::SetData(const uint32_t size, const void* data) {
		MIST_ASSERT(size != 0, "Trying to set uniform buffer data with a size of 0");
		MIST_ASSERT(data != nullptr, "Trying to set uniform buffer data with data that is a nullptr");

		VulkanContext& context = VulkanContext::GetContext();
		bool recreated = false;

		if (buffer.size != size) {
			Clear();
			VmaAllocationInfo info {};
			buffer.Create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT, info);
			mappedData = info.pMappedData;
			recreated = true;
		}

		memcpy(mappedData, data, size);
		vmaFlushAllocation(context.GetAllocator(), buffer.alloc, 0, size);
		return recreated;
	}

	void UniformBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();

		if (buffer.buffer != VK_NULL_HANDLE) {
			buffer.Clear();
			mappedData = nullptr;
		}
	}
}