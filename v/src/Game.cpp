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

		mist::ResourceManager* rm = mist::Application::Get().GetResourceManager();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->LoadEmptyScene();

		cameraEntity = sm->CreateEntity();
		mist::Transform& gameCameraT = sm->AddComponent<mist::Transform>(cameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& gameCamera = sm->AddComponent<mist::Camera>(cameraEntity, gameCameraT);
		gameCamera.SetPerspectiveCamera(1280, 720);

		//skyboxImage = rm->CreateImage("assets/HDR/testHDR.hdr", mist::TextureFormat::RGBA32F);
		skyboxShader = rm->CreateShader("assets/shaders/rollingskies.glsl");
		skyboxMat = rm->CreateMaterial(skyboxShader);
		//rm->SetTexture(renderData->GetRenderDataID(), skyboxMat, "skybox", skyboxImage);
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
		sm->UpdateSceneData(renderDataID, cam);
		sm->SubmitActiveSceneSkybox(renderDataID, skyboxMat);
		sm->SubmitActiveScene(renderDataID);
		renderAPI->EndRenderPass();
	}

	void Game::OnUpdate() {
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		float delta = mist::Application::Get().GetDeltaTime();

		// SDL_GetRelativeMouseState must be called every frame otherwise it breaks camera movement
		// when refocusing the window havent found a better solution yet
		glm::vec2 mouse;
		uint32_t buttons = SDL_GetRelativeMouseState(&mouse.x, &mouse.y);

		mist::Transform& transform = sm->GetComponent<mist::Transform>(cameraEntity);
		mouse *= 0.2;	// Sensitivity
		xRotation += mouse.y;
		xRotation = glm::clamp(xRotation, -90.0f, 90.0f);
		yRotation += mouse.x;

		glm::quat pitch = glm::angleAxis(glm::radians(xRotation), glm::vec3(1,0,0));
		glm::quat yaw = glm::angleAxis(glm::radians(yRotation), glm::vec3(0,1,0));
		transform.rotation = yaw * pitch;
		
		const bool* state = SDL_GetKeyboardState(NULL);
		glm::vec3 move = { 0, 0, 0 };
		if (state[SDL_SCANCODE_W])
			move.z += 1;
		if (state[SDL_SCANCODE_S])
			move.z -= 1;
		if (state[SDL_SCANCODE_D])
			move.x += 1;
		if (state[SDL_SCANCODE_A])
			move.x -= 1;
		if (state[SDL_SCANCODE_SPACE])
			move.y += 1;
		if (state[SDL_SCANCODE_LCTRL])
			move.y -= 1;
		if (state[SDL_SCANCODE_ESCAPE])
			ImGui::SetWindowFocus(NULL);

		transform.position += (
			transform.Forward() * move.z +
			transform.Up() * move.y +
			transform.Left() * move.x
		) * 15.0f * delta;
	}

	void Game::Resize(const uint32_t& x, const uint32_t& y) {
		renderData->Resize(x, y);
		parent->UpdateTexture(framebufferID, renderData);
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->GetComponent<mist::Camera>(cameraEntity).SetViewportSize(x, y);
	}
}