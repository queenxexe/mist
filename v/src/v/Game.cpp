#include "Game.hpp"
#include <Application.hpp>

namespace V {
    Game::Game(mist::ImguiLayer* layer) : parent(layer) {}

    void Game::Initialize() {
        std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8,
			mist::FramebufferTextureFormat::DEPTH32_STENCIL8
		};
		mist::FramebufferProperties properties;
		properties.type = mist::FramebufferType::SINGLE;
		properties.attachments = attachments;
		properties.width = 720;
		properties.height = 480;
		renderData = mist::RenderData::Create(properties);

        mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->LoadEmptyScene();

		cameraEntity = sm->CreateEntity();
		mist::Transform& gameCameraT = sm->AddComponent<mist::Transform>(cameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& gameCamera = sm->AddComponent<mist::Camera>(cameraEntity, gameCameraT);
		gameCamera.SetPerspectiveCamera(1280, 720);
    }

    void Game::Cleanup() {

    }

    void Game::OnImguiRender() {
        ImGui::Begin("Test");
		ImGui::Text("Hello!");
		ImGui::End();
    }

    void Game::OnRender() {
        mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		renderAPI->BeginRenderPass(renderData->GetRenderDataID());
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		mist::Camera& cam = dynamic_cast<mist::Camera&>(sm->GetComponent<mist::Camera>(cameraEntity));
		sm->UpdateSceneCamera(cam, renderData->GetRenderDataID());
		sm->SubmitActiveScene(renderData->GetRenderDataID());
		renderAPI->EndRenderPass();
    }

	void Game::OnUpdate() {
		
	}
}