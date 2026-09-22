#pragma once
#include "Core.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"
#include "renderer/RenderAPI.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Material.hpp"
#include "SceneManager.hpp"
#include "physics/Physics.hpp"

namespace mist {
	class Application {
	public:
		Application(const char* name = "Untitled");
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		static Application& Get();

		void Run();
		void Quit();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline const float GetDeltaTime() const { return deltaTime; }
		inline RenderAPI* GetRenderAPI() { return renderAPI; }
		inline Window* GetWindow() { return window; }
		inline const char* GetApplicationName() { return appName; }
		inline ShaderLibrary* GetShaderLibrary() { return &shaderLib; }
		inline MaterialLibrary* GetMaterialLibrary() { return &materialLib; }
		inline SceneManager* GetSceneManager() { return &sceneManager; }
	private:
		static Application* instance;
		const float maxDeltaTime = 0.05f;	// Stop weird issues if the game freezes, lags, etc.
		
		const char* appName = "Untitled";
		bool running = true;
		float deltaTime = 0;
		LayerStack layerStack;
		ShaderLibrary shaderLib;
		MaterialLibrary materialLib;
		Window* window;
		RenderAPI* renderAPI;
		SceneManager sceneManager;
		Physics physics;
	};
}