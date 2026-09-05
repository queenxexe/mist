#include "EditorLayer.hpp"
#include <Application.hpp>
#include <imgui.h>

namespace mistEditor {
	EditorLayer::EditorLayer(const mist::ImguiLayerConfig& config) : ImguiLayer(config), sceneWindow(this) {}

	EditorLayer::~EditorLayer() {}

	void EditorLayer::OnAttach() {
		ImguiLayer::OnAttach();
		sceneWindow.Initialize();
	}

	void EditorLayer::OnDetach() {
		sceneWindow.Cleanup();
		ImguiLayer::OnDetach();
	}

	void EditorLayer::OnUpdate() {
		sceneWindow.OnEditorUpdate();
	}

	void EditorLayer::OnRender() {
		mist::RenderAPI* api = mist::Application::Get().GetRenderAPI();
		api->BeginFrame();

		sceneWindow.OnRender();

		api->BeginRenderPass(renderData->GetRenderDataID());
		Begin();
		OnImguiRender();
		End();
		api->EndRenderPass();
		api->EndFrame();

		sceneWindow.PostRender();
	}

	void EditorLayer::OnImguiRender() {
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		} else {
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Crisp Dockspace", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
					NewScene();
				}
		
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
		
				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
					SaveSceneAs();
				}
		
				if (ImGui::MenuItem("Exit"))
					mist::Application::Get().Quit();
				ImGui::EndMenu();
			}
		
			ImGui::EndMenuBar();
		}

		sceneWindow.OnImguiRender();
		ImGui::End();
	}
	
	void EditorLayer::OnEvent(const SDL_Event* e) {
		ImguiLayer::OnEvent(e);
	}

	void EditorLayer::NewScene() {

	}

	void EditorLayer::OpenScene() {
	
	}

	void EditorLayer::SaveSceneAs() {
	
	}
}