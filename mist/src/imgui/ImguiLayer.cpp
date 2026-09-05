#include "imgui/ImguiLayer.hpp"
#include <vulkan/vulkan.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <vector>
#include <unordered_map>
#include "Application.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanRenderData.hpp"
#include <Debug.hpp>
#include <Utils.hpp>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

namespace mist {
	ImguiLayer::ImguiLayer(const ImguiLayerConfig& config) : Layer(config.name), config(config) {}

	ImguiLayer::~ImguiLayer() {}

	void ImguiLayer::OnAttach() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = config.flags;

		if (std::strcmp(config.fontPath, "") && mist::Utils::Exists(config.fontPath)) {
			ImFont* font = io.Fonts->AddFontFromFileTTF(config.fontPath, 16);
			io.FontDefault = font;
		}

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		SetDarkThemeColors();

		std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8
		};
		mist::FramebufferProperties properties;
		properties.type = FramebufferType::SWAPCHAIN;
		properties.attachments = attachments;
		properties.width = 1280;
		properties.height = 720;
		renderData = mist::RenderData::Create(properties);

		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			Ref<VulkanRenderData> data = std::dynamic_pointer_cast<VulkanRenderData>(renderData);

			ImGui_ImplSDL3_InitForVulkan(Application::Get().GetWindow()->GetNativeWindow());
			
			VulkanContext& context = VulkanContext::GetContext();
			QueueFamilyIndices indicies = context.FindQueueFamilies();
	
			VkSurfaceCapabilitiesKHR capabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.GetPhysicalDevice(), context.GetSurface(), &capabilities);
			uint32_t swapchainImageCount = capabilities.minImageCount;
			if (capabilities.maxImageCount > 0 && swapchainImageCount > capabilities.maxImageCount)
				swapchainImageCount = capabilities.maxImageCount;

			ImGui_ImplVulkan_InitInfo info = {};
			info.Instance = context.GetInstance();
			info.PhysicalDevice = context.GetPhysicalDevice();
			info.Device = context.GetDevice();
			info.QueueFamily = indicies.graphicsFamily.value();
			info.Queue = context.GetGraphicsQueue();
			info.PipelineCache = VK_NULL_HANDLE;
			info.DescriptorPool = data->descriptors.GetImGuiDescriptorPool();
			info.MinImageCount = capabilities.minImageCount;
			info.ImageCount = swapchainImageCount;
			info.Allocator = context.GetAllocationCallbacks();
			info.PipelineInfoMain.RenderPass = data->renderPass;
			info.PipelineInfoMain.Subpass = 0;
			info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			info.CheckVkResultFn = CheckVkResult;
			ImGui_ImplVulkan_Init(&info);
			ImGui_ImplVulkan_CreateMainPipeline(&info.PipelineInfoMain);
		} else {
			MIST_ERROR("Not implemented imgui backends for this render API");
		}
	}

	void ImguiLayer::OnDetach() {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			ImGui_ImplVulkan_Shutdown();
		}

		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImguiLayer::OnEvent(const SDL_Event* e) {
		ImGui_ImplSDL3_ProcessEvent(e);
	}

	void ImguiLayer::Begin() {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			ImGui_ImplVulkan_NewFrame();
		} else {
			MIST_ERROR("Not implemented imgui backends for this render API");
		}

		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		//ImGuizmo::BeginFrame(); This is not been added to the cmake and vcpkg yet but will mostly likely use this later
	}

	void ImguiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(
			(float)Application::Get().GetWindow()->GetWidth(), 
			(float)Application::Get().GetWindow()->GetHeight()
		);

		ImGui::Render();

		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			VulkanContext& context = VulkanContext::GetContext();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), context.GetCurrentFrameCommandBuffer());
		} else {
			MIST_ERROR("Not implemented imgui backends for this render API");
		}
	
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	ImTextureID ImguiLayer::AddTexture(const Ref<RenderData>& renderData) {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			Ref<VulkanRenderData> data = std::dynamic_pointer_cast<VulkanRenderData>(renderData);
			VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(data->GetFirstFramebufferImageView(), data->GetFirstFramebufferImageLayout());
			return reinterpret_cast<ImTextureID>(descriptorSet);
		}

		return ImTextureID_Invalid;
	}

	void ImguiLayer::UpdateTexture(ImTextureID& id, const Ref<RenderData>& renderData) {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			Ref<VulkanRenderData> data = std::dynamic_pointer_cast<VulkanRenderData>(renderData);
			VkDescriptorSet oldDescriptorSet = (VkDescriptorSet)(uintptr_t)id;
			ImGui_ImplVulkan_RemoveTexture(oldDescriptorSet);
			VkDescriptorSet newDescriptorSet = ImGui_ImplVulkan_AddTexture(data->GetFirstFramebufferImageView(), data->GetFirstFramebufferImageLayout());
			id = (ImTextureID)(uintptr_t)newDescriptorSet;
		}
	}

	void ImguiLayer::RemoveTexture(const ImTextureID& id) {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)(uintptr_t)id);
		}
	}

	void ImguiLayer::ImGuiImage(const ImTextureRef& texture, const ImVec2& imageSize, const ImVec2& uv0, const ImVec2& uv1) {
		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			ImGui::Image(texture, imageSize, uv0, uv1);
		}
	}

	void ImguiLayer::SetDarkThemeColors() {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabSelected] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabDimmed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabDimmedSelected] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title BG
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.95f, 0.1505f, 0.951f, 1.0f };
	}
}