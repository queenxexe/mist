#pragma once
#include <Math.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <renderer/Shader.hpp>
#include <components/Camera.hpp>
#include <imgui/ImguiLayer.hpp>
#include <entt/entt.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow(mist::ImguiLayer* layer);

		void Initialize();
		void OnEditorUpdate();
		void OnImguiRender();
		void OnRender();
		void PostRender();
		void Cleanup();
	private:
		bool focused = false;
		mist::ImguiLayer* parent;
		mist::Ref<mist::RenderData> renderData;
		entt::entity sceneCameraEntity;
		float xRotation = 0;
		float yRotation = 0;

		bool resizeRequested = false;
		mist::ImGuiTextureID sceneFramebufferID;
		glm::vec2 sceneViewportSize = { 0, 0 };

		mist::Ref<mist::Shader> testShader;
		std::vector<mist::Ref<mist::Mesh>> testMeshes;
	};
}