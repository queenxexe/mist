#pragma once
#include <imgui/ImguiLayer.hpp>
#include "Editor/SceneWindow.hpp"

namespace mistEditor {
	class EditorLayer : public mist::ImguiLayer {
	public:
		EditorLayer(const mist::ImguiLayerConfig& config);
		virtual ~EditorLayer() override;
	
		EditorLayer(const EditorLayer& other) = delete;
		EditorLayer& operator=(const EditorLayer& other) = delete;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnRender() override;
		virtual void OnEvent(const SDL_Event* e) override;
	private:
		void OnImguiRender();
		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		SceneWindow sceneWindow;
	};
}