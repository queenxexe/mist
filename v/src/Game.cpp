#include "Game.hpp"
#include <Application.hpp>

namespace V {
	Game::Game(mist::ImguiLayer* layer) : parent(layer) {}

	void Game::Initialize() {
		std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::TextureFormat::RGBA8,
			mist::TextureFormat::DEPTH32_STENCIL8
		};
		mist::FramebufferProperties properties;
		properties.type = mist::FramebufferType::SINGLE;
		properties.attachments = attachments;
		properties.width = 1280;
		properties.height = 720;
		renderData = mist::RenderData::Create(properties);
		framebufferID = parent->AddTexture(renderData);

		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->LoadEmptyScene();

		cameraEntity = sm->CreateEntity();
		mist::Transform& gameCameraT = sm->AddComponent<mist::Transform>(cameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& gameCamera = sm->AddComponent<mist::Camera>(cameraEntity, gameCameraT);
		gameCamera.SetPerspectiveCamera(1280, 720);

		skyboxImage = mist::Image::Create("assets/testHDR.hdr", mist::TextureFormat::RGBA32F);
		skyboxShader = mist::Application::Get().GetShaderLibrary()->Load("assets/shaders/skybox.glsl");
		skyboxMat = mist::Application::Get().GetMaterialLibrary()->Create(skyboxShader);
		skyboxMat->SetTexture(renderData->GetRenderDataID(), "skybox", skyboxImage);

		// skyboxShader = mist::Application::Get().GetShaderLibrary()->Load("assets/shaders/fullscreenTest.glsl");
		// skyboxMat = mist::Application::Get().GetMaterialLibrary()->Create(skyboxShader);
	}

	void Game::Cleanup() {}

	void Game::OnImguiRender() {
		ImGui::Begin("Test");
		ImGui::Text("Hello!");
		ImGui::End();
	}

	void Game::OnRender() {
		uint8_t renderDataID = renderData->GetRenderDataID();
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();

		renderAPI->BeginRenderPass(renderDataID);
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		mist::Camera& cam = dynamic_cast<mist::Camera&>(sm->GetComponent<mist::Camera>(cameraEntity));
		sm->UpdateSceneCamera(cam, renderDataID);
		sm->SubmitActiveSceneSkybox(renderDataID, skyboxMat);
		sm->SubmitActiveScene(renderDataID);
		renderAPI->EndRenderPass();
	}

	void Game::OnUpdate() {}

	void Game::Resize(const uint32_t& x, const uint32_t& y) {
		renderData->Resize(x, y);
		parent->UpdateTexture(framebufferID, renderData);
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->GetComponent<mist::Camera>(cameraEntity).SetViewportSize(x, y);
	}
}