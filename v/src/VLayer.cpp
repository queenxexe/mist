#include "VLayer.hpp"
#include <Application.hpp>
#include <imgui.h>

namespace V {
	VLayer::VLayer(const mist::ImguiLayerConfig& config) : ImguiLayer(config), game(this) {}

	VLayer::~VLayer() {}

	void VLayer::OnAttach() {
		ImguiLayer::OnAttach();
		game.Initialize();
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
	}

	void VLayer::OnImguiRender() {
		game.OnImguiRender();
	}
}