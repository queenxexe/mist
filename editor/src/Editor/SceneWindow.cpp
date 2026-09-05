#include "SceneWindow.hpp"
#include <renderer/Buffer.hpp>
#include <vector>
#include <Application.hpp>
#include <components/MeshRenderer.hpp>
#include <components/Collider.hpp>
#include <components/Rigidbody.hpp>
#include <components/DirectionalLight.hpp>
#include <data/Importer.hpp>

namespace mistEditor {
	SceneWindow::SceneWindow(mist::ImguiLayer* layer) : parent(layer) {}

	void SceneWindow::Initialize() {
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
		sceneFramebufferID = parent->AddTexture(renderData);

		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->LoadEmptyScene();

		sceneCameraEntity = sm->CreateEntity();
		mist::Transform& sceneCameraT = sm->AddComponent<mist::Transform>(sceneCameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& sceneCamera = sm->AddComponent<mist::SceneCamera>(sceneCameraEntity, sceneCameraT);
		sceneCamera.SetPerspectiveCamera(1280, 720);

		// GAME
		testShader = mist::Application::Get().GetShaderLibrary()->Load("assets/shaders/lambert.glsl");

		//testMeshes = mist::Importer::ImportMeshes("assets/brassfang.fbx");
		testMeshes = mist::Importer::ImportMeshes("assets/LightCycle.obj", true);
		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(-2, 0, 0), glm::quat_identity<float, glm::defaultp>(), glm::vec3(1.0f));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMeshes[0]);
		}

		std::vector<mist::Vertex> verts = {
			{{ -1.0f, -1.0f, 0.0f }, {0,0,0}},
			{{ -1.0f,  1.0f, 0.0f }, {0,0,0}},
			{{  1.0f, -1.0f, 0.0f }, {0,0,0}},
			{{  1.0f,  1.0f, 0.0f }, {0,0,0}}
		};
		
		std::vector<uint32_t> indices = {
			0, 1, 2,
			3, 2, 1
		};
		
		mist::Ref<mist::Mesh> testMesh = mist::CreateRef<mist::Mesh>(verts, indices);
		testMesh->GenerateNormals();
		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(2, 0, 0));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMesh);
		}

		const entt::entity gameCameraEntity = sm->CreateEntity();
		mist::Transform& gameCameraT = sm->AddComponent<mist::Transform>(gameCameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& gameCamera = sm->AddComponent<mist::Camera>(gameCameraEntity, gameCameraT);
		gameCamera.SetPerspectiveCamera(1280, 720);

		const entt::entity directionalLightEntity = sm->CreateEntity();
		mist::Transform& directionalLightT = sm->AddComponent<mist::Transform>(directionalLightEntity, glm::vec3(0, 0, -5), glm::quat(glm::radians(glm::vec3(-45, 180, 0))));
		mist::DirectionalLight& directionalLight = sm->AddComponent<mist::DirectionalLight>(directionalLightEntity, directionalLightT, glm::vec3(1,1,1));
	}

	void SceneWindow::Cleanup() {
		testShader->Clear();
	}

	void SceneWindow::OnEditorUpdate() {
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		float delta = mist::Application::Get().GetDeltaTime();

		auto view = sm->GetActiveScene().view<mist::Transform, mist::MeshRenderer>();
		view.each([delta](mist::Transform &transform, mist::MeshRenderer &render) {
			transform.Rotate(glm::radians(30.0f) * delta, { 0, 1, 0 });
		});

		// SDL_GetRelativeMouseState must be called every frame otherwise it breaks camera movement
		// when refocusing the window havent found a better solution yet
		glm::vec2 mouse;
		uint32_t buttons = SDL_GetRelativeMouseState(&mouse.x, &mouse.y);

		if (!focused)
			return;

		mist::Transform& transform = sm->GetComponent<mist::Transform>(sceneCameraEntity);
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

	void SceneWindow::OnImguiRender() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (sceneViewportSize != *((glm::vec2*)&viewportSize)) {
			resizeRequested = true;
			sceneViewportSize = { viewportSize.x, viewportSize.y };
		}

		ImVec2 imageSize = {
			static_cast<float>(renderData->GetProperties().width),
			static_cast<float>(renderData->GetProperties().height)
		};
		parent->ImGuiImage(sceneFramebufferID, imageSize, ImVec2{0, 0}, ImVec2{1, 1});

		focused = ImGui::IsWindowFocused(ImGuiWindowFlags_NoFocusOnAppearing);
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneWindow::OnRender() {
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		renderAPI->BeginRenderPass(renderData->GetRenderDataID());
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		mist::Camera& cam = dynamic_cast<mist::Camera&>(sm->GetComponent<mist::SceneCamera>(sceneCameraEntity));
		sm->UpdateSceneCamera(cam, renderData->GetRenderDataID());
		sm->SubmitActiveScene(renderData->GetRenderDataID());
		renderAPI->EndRenderPass();
	}

	void SceneWindow::PostRender() {
		if (resizeRequested) {
			renderData->Resize(sceneViewportSize.x, sceneViewportSize.y);
			parent->UpdateTexture(sceneFramebufferID, renderData);
			mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
			sm->GetComponent<mist::SceneCamera>(sceneCameraEntity).SetViewportSize(sceneViewportSize.x, sceneViewportSize.y);
			resizeRequested = false;
		}
	}
}