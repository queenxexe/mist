#include "SceneManager.hpp"
#include "components/Transform.hpp"
#include "components/DirectionalLight.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "ResourceManagerInternal.hpp"

namespace mist {
	const entt::entity SceneManager::CreateEntity() {
		const entt::entity entity = loadedScenes[activeScene].create();
		return entity;
	}

	void SceneManager::DestroyEntity(const entt::entity entity) {
		loadedScenes[activeScene].destroy(entity);
	}

	void SceneManager::SubmitSkybox(const uint8_t renderDataID, const int32_t sceneIndex, const MaterialRef& skyboxMaterial) {
		ResourceManager* rm = Application::Get().GetResourceManager();
		ShaderRef shader = ResourceManagerInternal::GetMaterial(rm, skyboxMaterial.id)->GetShaderRef();
		ResourceManagerInternal::Bind(rm, renderDataID, shader);
		ResourceManagerInternal::Bind(rm, renderDataID, skyboxMaterial);
		Application::Get().GetRenderAPI()->DrawFullscreen();
	}

	void SceneManager::SubmitScene(const uint8_t renderDataID, const int32_t sceneIndex) {
		auto lightView = loadedScenes[sceneIndex].view<DirectionalLight>();
		for (auto entity : lightView) {
			Application::Get().GetRenderAPI()->UpdateDirectionalLight(renderDataID, loadedScenes[activeScene].get<DirectionalLight>(entity));
			break;	// Only pass the first directional light as there should only be 1
		}
		
		ResourceManager* rm = Application::Get().GetResourceManager();
		auto view = loadedScenes[sceneIndex].view<MeshRenderer, MaterialRef>();
		
		// Binding and unbinding a shader pipeline after each object is terrible but will do for testing sake
		// ideally we bind a shader then render everything with that shader before moving on
		// unless there is better methods im unaware of
		ResourceID currentShaderID = INVALID_RESOURCE_ID;
		view.each([renderDataID, rm, &currentShaderID](MeshRenderer& renderer, MaterialRef& materialRef) {
			std::shared_ptr<Material> material = ResourceManagerInternal::GetMaterial(rm, materialRef.id);
			ShaderRef shader = material->GetShaderRef();
			
			if (shader.id != currentShaderID) {
				ResourceManagerInternal::Bind(rm, renderDataID, shader);
				currentShaderID = shader.id;
			}
			
			material->Bind(renderDataID);
			glm::mat4 matrix = renderer.GetTransform().GetLocalToWorldMatrix();
			material->SetPushConstant(renderDataID, "ModelMatrix", sizeof(matrix), &matrix);
			renderer.Bind(renderDataID);
			renderer.Draw();
		});
	}

	void SceneManager::UpdateSceneCamera(const Camera& camera, const uint8_t renderDataID) {
		Application::Get().GetRenderAPI()->UpdateCamera(renderDataID, camera);
	}

	void SceneManager::UpdateSceneCamera(const uint8_t renderDataID) {
		auto camView = loadedScenes[activeScene].view<Camera>();
		
		Camera* cam;
		for (auto entity : camView) {
			cam = loadedScenes[activeScene].try_get<Camera>(entity);

			if (cam != nullptr)
				break;
		}

		if (cam != nullptr) {
			Application::Get().GetRenderAPI()->UpdateCamera(renderDataID, *cam);
			return;
		}

		MIST_WARN("No camera in active scene");
	}

	void SceneManager::LoadEmptyScene() {
		loadedScenes.push_back(entt::registry{});
		MIST_INFO("Loaded empty scene");
		
		if (activeScene == -1)
			SetActiveScene(0);
	}

	void SceneManager::LoadScene() {
		MIST_WARN("Currently not implemented");
	}

	void SceneManager::SetActiveScene(const int32_t sceneIndex) {
		activeScene = sceneIndex;
		MIST_INFO("Set active scene to {}", sceneIndex);
	}

	void SceneManager::Cleanup() {
		for (size_t i = 0; i < loadedScenes.size(); ++i) {
			auto view = loadedScenes[i].view<MeshRenderer>();
			view.each([](MeshRenderer &renderer) {
				renderer.Clear();
			});
			
			loadedScenes[i].clear();
		}
	}
}