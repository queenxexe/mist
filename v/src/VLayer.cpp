#include "VLayer.hpp"
#include <Application.hpp>
#include <imgui.h>

namespace V {
	VLayer::VLayer(const mist::ImguiLayerConfig& config) : ImguiLayer(config), game(this) {}

	VLayer::~VLayer() {}

	void VLayer::OnUpdate() {
		
	}

	void VLayer::OnRender() {
		mist::RenderAPI* api = mist::Application::Get().GetRenderAPI();
		api->BeginFrame();
		api->BeginRenderPass(renderData->GetRenderDataID());
		Begin();
		OnImguiRender();
		End();
		api->EndRenderPass();
		api->EndFrame();
	}

	void VLayer::OnImguiRender() {
		ImGui::Begin("Test");
		ImGui::Text("Hello!");
		ImGui::End();
	}
}