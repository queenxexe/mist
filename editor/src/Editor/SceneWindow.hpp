#pragma once
#include <memory>
#include <Math.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <components/Camera.hpp>
#include <imgui/ImguiLayer.hpp>
#include <entt/entt.hpp>
#include <data/RefTypes.hpp>

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
		std::shared_ptr<mist::RenderData> renderData;
		entt::entity sceneCameraEntity;
		float xRotation = 0;
		float yRotation = 0;

		bool resizeRequested = false;
		ImTextureID sceneFramebufferID;
		glm::vec2 sceneViewportSize = { 0, 0 };

		mist::ShaderRef testShader;
		mist::MaterialRef material; 
		mist::ImageRef skyboxImage;
		mist::ShaderRef skyboxShader;
		mist::MaterialRef skyboxMat;
		std::vector<std::shared_ptr<mist::Mesh>> testMeshes;
	};
}