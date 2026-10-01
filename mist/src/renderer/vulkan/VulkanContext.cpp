#include "VulkanContext.hpp"
#include <Math.hpp>
#include <set>
#include <SDL3/SDL_vulkan.h>
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "renderer/vulkan/VulkanRenderData.hpp"

namespace mist {
	void FrameData::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		if (acquireImageSempahore != VK_NULL_HANDLE)
			vkDestroySemaphore(context.GetDevice(), acquireImageSempahore, context.GetAllocationCallbacks());
			
		if (inFlightFence != VK_NULL_HANDLE)
			vkDestroyFence(context.GetDevice(), inFlightFence, context.GetAllocationCallbacks());
	}

	const QueueFamilyIndices VulkanContext::FindQueueFamilies() const {
		QueueFamilyIndices indices;

		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues);

		for (uint32_t i = 0; i < count; i++) {
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport)
				indices.presentFamily = i;

			if (indices.Valid())
				break;
		}
		
		return indices;
	}

	uint32_t VulkanContext::FindMemoryType(uint32_t typeFiler, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            if (typeFiler & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
                return i;
        }

        MIST_ERROR("Failed to find suitable memory type");
        return 0;
    }

	std::vector<VkSurfaceFormatKHR> QuerySwapchainFormats(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
		uint32_t count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
		return formats;
	}

	VkSurfaceFormatKHR ChooseSwapchainFormat(const VkPhysicalDevice phyiscalDevice, const VkSurfaceKHR surface, const TextureFormat& colorFormat) {
		MIST_ASSERT(VulkanHelper::IsColorFormat(colorFormat), "Trying to create swapchain with a non color format.");
		std::vector<VkSurfaceFormatKHR> availableFormats = QuerySwapchainFormats(phyiscalDevice, surface);

		VkSurfaceFormatKHR preferedFormat;
		preferedFormat.format = VulkanHelper::GetVkFormat(colorFormat);
		preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		for (const VkSurfaceFormatKHR& format : availableFormats) {
			if (format.format == preferedFormat.format && format.colorSpace == preferedFormat.colorSpace) {
				return preferedFormat;
			}
		}
		
		MIST_INFO("No valid and supported swapchain format given, using a fallback from available formats.");
		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapchainPresentMode(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const RenderAPI::VSYNC vsync) {
		VkPresentModeKHR preferredMode = VulkanHelper::GetPresentMode(vsync);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, availablePresentModes.data());

		if (std::find(availablePresentModes.begin(), availablePresentModes.end(), preferredMode) != availablePresentModes.end())
			return preferredMode;

		return availablePresentModes[0];
	}

	VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR capabilities, const uint32_t width, const uint32_t height) {
		return {
			std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, width)),
			std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, height))
		};
	}

	void VulkanContext::CreateInstance() {
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = Application::Get().GetApplicationName();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "mist";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		std::vector<const char*> requestedExtensions = {};
		unsigned int count = 0;
		const char* const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
		requestedExtensions.reserve(count);
		for (size_t i = 0; i < count; i++) {
			requestedExtensions.emplace_back(sdlExtensions[i]);
		}

#ifdef DEBUG
		requestedExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::vector<const char*> validExtensions = {};
		for (const auto& extension : requestedExtensions) {
			bool found = false;
			for (const auto& availableExtension : availableExtensions) {
				if (strcmp(extension, availableExtension.extensionName) == 0) {
					validExtensions.push_back(extension);
					found = true;
					break;
				}
			}

			if (!found)
				MIST_WARN(std::string("Unavailable extension: ") + extension);
		}

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.ppEnabledExtensionNames = validExtensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(validExtensions.size());
		
#ifdef DEBUG
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::vector<const char*> requestedLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		std::vector<const char*> validLayers = {};
		for (const auto& layer : requestedLayers) {
			bool found = false;
			for (const auto& availableLayer : availableLayers) {
				if (strcmp(layer, availableLayer.layerName) == 0) {
					validLayers.push_back(layer);
					found = true;
					break;
				}
			}

			if (!found)
				MIST_WARN(std::string("Unavailable layer: ") + layer);
		}

		createInfo.ppEnabledLayerNames = validLayers.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validLayers.size());
#endif
		
		CheckVkResult(vkCreateInstance(&createInfo, allocationCallbacks, &instance));
		
#ifdef DEBUG
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI = {};
		debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCI.messageSeverity =
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		debugUtilsMessengerCI.messageType =
		    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsMessengerCI.pfnUserCallback = DebugUtilsMessengerCallback;
		debugUtilsMessengerCI.pUserData = nullptr;

		CheckVkResult(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, allocationCallbacks, &debugMessenger));
#endif
	}

	void VulkanContext::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface(Application::Get().GetWindow()->GetNativeWindow(), instance, allocationCallbacks, &surface))
			MIST_ERROR("Surface failed to be created");
	}

	void VulkanContext::CreatePhysicalDevice() {
		uint32_t gpuCount;
		CheckVkResult(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
		MIST_ASSERT(gpuCount > 0, "Failed to find any GPUs");

		VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpuCount);
		CheckVkResult(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus));

		// TODO: Add support for multi dedicated GPUs
		// Finds a discrete GPU
		uint32_t gpuIdx = 0;
		for (uint32_t i = 0; i < gpuCount; i++) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				gpuIdx = i;
				break;
			}
		}

		physicalDevice = gpus[gpuIdx];
		free(gpus);
	}

	void VulkanContext::CreateDevice() {
		QueueFamilyIndices indicies = FindQueueFamilies();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = queueFamily;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(createInfo);
		}
		
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures deviceFeatures{};
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		CheckVkResult(vkCreateDevice(physicalDevice, &deviceCreateInfo, allocationCallbacks, &device));
		vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
	}

	void VulkanContext::CreateAllocator() {
		VmaAllocatorCreateInfo info {};
		info.instance = instance;
		info.physicalDevice = physicalDevice;
		info.device = device;

		CheckVkResult(vmaCreateAllocator(&info, &allocator));
	}

	void VulkanContext::CreateFrameDatas() {
		for (FrameData& data : frameDatas)
			data.Cleanup();
		frameDatas.clear();
		frameDatas.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			CheckVkResult(vkCreateSemaphore(device, &semaphoreInfo, allocationCallbacks, &frameDatas[i].acquireImageSempahore));
			CheckVkResult(vkCreateFence(device, &fenceInfo, allocationCallbacks, &frameDatas[i].inFlightFence));
		}
	}

	void VulkanContext::Initialize() {
		CreateInstance();
		CreateSurface();
		CreatePhysicalDevice();
		CreateDevice();
		CreateAllocator();
		CreateCommandPool();
		AllocateCommandBuffers();
		CreateFrameDatas();
		MIST_INFO("Initialised Vulkan API");
	}

	void VulkanContext::Cleanup() {
		Application::Get().GetResourceManager()->Cleanup();

		for (std::pair<const uint8_t, std::shared_ptr<VulkanRenderData>>& data : renderDatas)
			data.second->Cleanup();

		for (VkImageView& swapchainImageView : swapchainImageViews)
			vkDestroyImageView(device, swapchainImageView, allocationCallbacks);

		for (VkSemaphore& semaphore : submitSemaphores)
			vkDestroySemaphore(device, semaphore, allocationCallbacks);

		if (swapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(device, swapchain, allocationCallbacks);

		if (commandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, commandPool, allocationCallbacks);

		for (FrameData& data : frameDatas)
			data.Cleanup();

		if (tempCommandBufferFence != VK_NULL_HANDLE)
			vkDestroyFence(device, tempCommandBufferFence, allocationCallbacks);

		vmaDestroyAllocator(allocator);
		vkDestroyDevice(device, allocationCallbacks);
		SDL_Vulkan_DestroySurface(instance, surface, allocationCallbacks);
#ifdef DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocationCallbacks);
#endif
		vkDestroyInstance(instance, allocationCallbacks);
		MIST_INFO("Cleaned up Vulkan API");
	}

	void VulkanContext::CreateCommandPool() {
		if (commandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, commandPool, allocationCallbacks);
		
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = FindQueueFamilies().graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CheckVkResult(vkCreateCommandPool(device, &poolInfo, allocationCallbacks, &commandPool));
	}

	void VulkanContext::AllocateCommandBuffers() {
		commandBuffers.clear();
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		CheckVkResult(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

		VkCommandBufferAllocateInfo tempAllocInfo {};
		tempAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		tempAllocInfo.commandPool = commandPool;
		tempAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		tempAllocInfo.commandBufferCount = 1;

		CheckVkResult(vkAllocateCommandBuffers(device, &tempAllocInfo, &tempCommandBuffer));

		if (tempCommandBufferFence != VK_NULL_HANDLE)
			vkDestroyFence(device, tempCommandBufferFence, allocationCallbacks);

		VkFenceCreateInfo tempCommandBufferFenceInfo{};
		tempCommandBufferFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		CheckVkResult(vkCreateFence(device, &tempCommandBufferFenceInfo, allocationCallbacks, &tempCommandBufferFence));
	}

	void VulkanContext::CreateSwapchain(const uint8_t renderDataID, const FramebufferProperties& properties) {
		swapchainRenderDataID = renderDataID;
		VkSurfaceFormatKHR selectedFormat = ChooseSwapchainFormat(physicalDevice, surface, properties.attachments[0].textureFormat);
		
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		uint32_t swapchainImageCount = capabilities.minImageCount;
		if (capabilities.maxImageCount > 0 && swapchainImageCount > capabilities.maxImageCount)
			swapchainImageCount = capabilities.maxImageCount;

		for (VkSemaphore& semaphore : submitSemaphores)
			vkDestroySemaphore(device, semaphore, allocationCallbacks);
		submitSemaphores.clear();
		submitSemaphores.resize(swapchainImageCount);

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (size_t i = 0; i < swapchainImageCount; ++i)
			vkCreateSemaphore(device, &semaphoreInfo, allocationCallbacks, &submitSemaphores[i]);

		VkSwapchainKHR oldSwapchain = swapchain;

		VkExtent2D extent = ChooseSwapchainExtent(capabilities, properties.width, properties.height);

		VkSwapchainCreateInfoKHR swapchainInfo {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = surface;
		swapchainInfo.imageFormat = selectedFormat.format;
		swapchainInfo.imageColorSpace = selectedFormat.colorSpace;
		swapchainInfo.presentMode = ChooseSwapchainPresentMode(physicalDevice, surface, Application::Get().GetRenderAPI()->GetVsyncMode());
		swapchainInfo.imageExtent = extent;
		swapchainInfo.minImageCount = swapchainImageCount;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.preTransform = capabilities.currentTransform;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = oldSwapchain;

		QueueFamilyIndices indicies = FindQueueFamilies();
		uint32_t queueFamilyIndices[] = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
		if (indicies.graphicsFamily != indicies.presentFamily) {
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainInfo.queueFamilyIndexCount = 2;
			swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		CheckVkResult(vkCreateSwapchainKHR(device, &swapchainInfo, allocationCallbacks, &swapchain));
		if (oldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(device, oldSwapchain, allocationCallbacks);

		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());

		for (VkImageView view : swapchainImageViews)
			vkDestroyImageView(device, view, allocationCallbacks);
		swapchainImageViews.clear();
		swapchainImageViews.resize(swapchainImageCount);

		VkImageViewCreateInfo imageViewInfo {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		
		for (uint32_t i = 0; i < swapchainImageCount; ++i) {
			imageViewInfo.image = swapchainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = selectedFormat.format;
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			CheckVkResult(vkCreateImageView(device, &imageViewInfo, allocationCallbacks, &swapchainImageViews[i]));
		}
	}

	void VulkanContext::RecreateSwapchain(const uint32_t width, const uint32_t height) {
		if (swapchainRenderDataID == INVALID_RENDER_DATA_ID)
			return;

		MIST_INFO("Recreating Swapchain");
		vkDeviceWaitIdle(VulkanContext::GetContext().GetDevice());

		std::shared_ptr<VulkanRenderData>& swapchainRenderData = renderDatas[swapchainRenderDataID];
		FramebufferProperties newProp = swapchainRenderData->GetProperties();
		newProp.width = width;
		newProp.height = height;

		swapchainRenderData->Reset();
		CreateSwapchain(swapchainRenderDataID, newProp);
		renderDatas[swapchainRenderDataID]->Resize(width, height);
	}

	void VulkanContext::RecreateSwapchain() {
		if (swapchainRenderDataID == INVALID_RENDER_DATA_ID)
			return;

		std::shared_ptr<VulkanRenderData>& swapchainRenderData = renderDatas[swapchainRenderDataID];
		FramebufferProperties props = swapchainRenderData->GetProperties();
		RecreateSwapchain(props.width, props.height);
	}

	void VulkanContext::BeginSingleTimeCommands() {
		CheckVkResult(vkResetCommandBuffer(tempCommandBuffer, 0));

		VkCommandBufferBeginInfo info {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CheckVkResult(vkBeginCommandBuffer(tempCommandBuffer, &info));
	}

	void VulkanContext::EndSingleTimeCommands() {
		CheckVkResult(vkEndCommandBuffer(tempCommandBuffer));

		VkSubmitInfo info {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &tempCommandBuffer;
		
		CheckVkResult(vkQueueSubmit(graphicsQueue, 1, &info, tempCommandBufferFence));
		CheckVkResult(vkWaitForFences(device, 1, &tempCommandBufferFence, VK_TRUE, UINT64_MAX));
		CheckVkResult(vkResetFences(device, 1, &tempCommandBufferFence));
	}

	void VulkanContext::BeginFrame() {
		CheckVkResult(vkWaitForFences(device, 1, &frameDatas[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX));
 		CheckVkResult(vkResetFences(device, 1, &frameDatas[currentFrame].inFlightFence));

		VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frameDatas[currentFrame].acquireImageSempahore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			RecreateSwapchain();
			BeginFrame();
			return;
		} else if (result != VK_SUCCESS) {
			CheckVkResult(result);
		}

		CheckVkResult(vkResetCommandBuffer(commandBuffers[currentFrame], 0));
		VkCommandBufferBeginInfo cmdInfo {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CheckVkResult(vkBeginCommandBuffer(commandBuffers[currentFrame], &cmdInfo));
	}

	void VulkanContext::EndFrame() {
		CheckVkResult(vkEndCommandBuffer(commandBuffers[currentFrame]));

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frameDatas[currentFrame].acquireImageSempahore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &submitSemaphores[imageIndex];

		CheckVkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameDatas[currentFrame].inFlightFence));

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &submitSemaphores[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
			RecreateSwapchain();
		} else if (presentResult != VK_SUCCESS) {
			CheckVkResult(presentResult);
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanContext::BeginRenderPass(const uint8_t renderDataID) {
		std::shared_ptr<VulkanRenderData> data = renderDatas[renderDataID]; 
		uint32_t index = imageIndex;
		if (data->GetProperties().type != FramebufferType::SWAPCHAIN)
			index = 0;

		glm::vec4 color = Application::Get().GetRenderAPI()->GetClearColor();
		VkClearValue clearColor {};
		clearColor.color = { color.r, color.g, color.b, color.a };
		VkClearValue depthValue {};
		depthValue.depthStencil = { 1.0, 0 };
		
		std::vector<VkClearValue> clearValues;
		if (data->GetProperties().type == FramebufferType::SWAPCHAIN)
			clearValues.push_back(clearColor);	// Swapchain image
			
		for (FramebufferAttachment& attachment : data->framebufferAttachments[index])
			clearValues.push_back(attachment.isDepth ? depthValue : clearColor); 
			
		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = data->renderPass;
		renderPassInfo.framebuffer = data->framebuffers[index];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = data->scissor.extent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		// Using dynamic viewport and scissor so needs to be set every frame
		vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &data->viewport);
		vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &data->scissor);
	}

	void VulkanContext::EndRenderPass() {
		vkCmdEndRenderPass(commandBuffers[currentFrame]);
	}

	std::shared_ptr<VulkanRenderData> VulkanContext::CreateNewRenderData() {
		uint8_t id = GetNewRenderDataID();
		renderDatas.emplace(id, std::make_shared<VulkanRenderData>(id));
		return renderDatas[id];
	}

	uint8_t VulkanContext::GetNewRenderDataID() {
		uint8_t id = renderDataCounter;
		renderDataCounter++;
		return id;
	}
}