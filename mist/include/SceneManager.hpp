#pragma once
#include <entt/entt.hpp>
#include "renderer/Shader.hpp"
#include "renderer/Material.hpp"
#include "components/Camera.hpp"

namespace mist {
	class SceneManager {
	public:
		const entt::entity CreateEntity();
		void DestroyEntity(const entt::entity entity);

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args) {
			return loadedScenes[activeScene].emplace<T>(std::forward<Args>(args)...);
		}
		
		template<typename T>
		void RemoveComponent(const entt::entity entity) {
			loadedScenes[activeScene].erase<T>(entity);
		}

		template<typename T>
		T& GetComponent(const entt::entity entity) {
			return loadedScenes[activeScene].get<T>(entity);
		}
		
		inline const int32_t GetActiveSceneIndex() { return activeScene; }
		inline entt::registry& GetActiveScene() { return loadedScenes[activeScene]; }

		inline void SubmitActiveScene(const uint8_t renderDataID) { SubmitScene(renderDataID, activeScene); }
		void SubmitScene(const uint8_t renderDataID, const int32_t sceneIndex);

		inline void SubmitActiveSceneSkybox(const uint8_t renderDataID, const Ref<Material>& skyboxMaterial) { SubmitSkybox(renderDataID, activeScene, skyboxMaterial); }
		void SubmitSkybox(const uint8_t renderDataID, const int32_t sceneIndex, const Ref<Material>& skyboxMaterial);

		void UpdateSceneCamera(const Camera& camera, const uint8_t renderDataID);
		void UpdateSceneCamera(const uint8_t renderDataID);

		void LoadEmptyScene();
		void LoadScene();
		void SetActiveScene(const int32_t sceneIndex);

		void Cleanup();
	private:
		int32_t activeScene = -1;
		std::vector<entt::registry> loadedScenes;
	};
}