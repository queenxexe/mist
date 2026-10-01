#include "VLayer.hpp"
#include <Application.hpp>
#include <imgui.h>
#include <Log.hpp>

namespace V {
	VLayer::VLayer(const mist::ImguiLayerConfig& config) : ImguiLayer(config), game(this) {}

	VLayer::~VLayer() {}

	void VLayer::OnAttach() {
		ImguiLayer::OnAttach();
		game.Initialize();
		windowSize = mist::Application::Get().GetWindow()->GetSize();
	}

	void VLayer::OnDetach() {
		game.Cleanup();
		ImguiLayer::OnDetach();
	}

	void VLayer::OnUpdate() {
		game.OnUpdate();
	}

	void VLayer::OnRender() {
		mist::RenderAPI* api = mist::Application::Get().GetRenderAPI();
		api->BeginFrame();

		game.OnRender();

		api->BeginRenderPass(renderData->GetRenderDataID());
		Begin();
		OnImguiRender();
		End();
		api->EndRenderPass();
		api->EndFrame();

		PostRender();
	}

	void VLayer::OnImguiRender() {
		ImGuiWindowFlags window_flags = 
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoResize;
		
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Game Dockspace", nullptr, window_flags);

		ImVec2 pos = ImGui::GetWindowPos();
		glm::ivec2 size = mist::Application::Get().GetWindow()->GetSize();
		if (windowSize != size) {
			resizeRequested = true;
			windowSize = size;
		}
		
		ImGuiID dockspace_id = ImGui::GetID("Dockspace");
		ImGui::GetWindowDrawList()->AddImage(game.GetFramebufferID(), pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		
		ImGui::End();
		ImGui::PopStyleVar(2);

		game.OnImguiRender();
	}

	void VLayer::PostRender() {
		if (resizeRequested) {
			APP_INFO("Resize");
			game.Resize(windowSize.x, windowSize.y);
			resizeRequested = false;
		}
	}
}