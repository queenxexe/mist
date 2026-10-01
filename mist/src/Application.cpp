#define SDL_MAIN_HANDLED

#include "Application.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <stdexcept>
#include <chrono>
#include "renderer/vulkan/VulkanRenderAPI.hpp"
#include "Log.hpp"

namespace mist {
	Application* Application::instance = nullptr;

	Application::Application(const char* name) {
		Log::Init();

		if (instance != nullptr)
			throw std::runtime_error("Created 2 application instances");

		instance = this;
		appName = name;
		renderAPI = new VulkanRenderAPI();
		SDL_Init(SDL_INIT_VIDEO);
		window = Window::Create(WindowProperties(name));
		renderAPI->Initialize();
	}

	Application::~Application() {
		renderAPI->WaitForIdle();
		layerStack.Clear();
		sceneManager.Cleanup();
		renderAPI->Shutdown();
		SDL_Quit();
	}

	Application& Application::Get() {
		if (instance == nullptr)
			instance = new Application();
		
		return *instance;
	}

	void Application::Run() {
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
		while (running) {
			SDL_Event event;
			while (SDL_PollEvent (&event)) {
				switch (event.type) {
				case SDL_EVENT_WINDOW_RESIZED:
				{
					uint32_t newWidth = event.window.data1;
					uint32_t newHeight = event.window.data2;
					if (newWidth > 0 && newHeight > 0)
						renderAPI->Resize(newWidth, newHeight);
					break;
				}
				case SDL_EVENT_QUIT:
					Quit();
					break;
				}

				for (Layer* layer : layerStack) {
					layer->OnEvent(&event);
				}
			}
			
			for (Layer* layer : layerStack) {
				layer->OnUpdate();
			}

			physics.Simulate(deltaTime);
			
			for (Layer* layer : layerStack) {
				layer->OnRender();
			}

			std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
			deltaTime = std::min(std::chrono::duration<float>(currentTime - lastTime).count(), maxDeltaTime);
			lastTime = currentTime;
		}
	}

	void Application::Quit() {
		running = false;
	}

	void Application::PushLayer(Layer* layer) {
		layerStack.PushLayer(layer);
	}

	void Application::PopLayer(Layer* layer) {
		layerStack.PopLayer(layer);
	}
}