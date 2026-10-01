#pragma once
#include <Math.hpp>
#include <SDL3/SDL.h>
#define INIT_WIDTH 720
#define INIT_HEIGHT 480

namespace mist {
	struct WindowProperties {
		const char* title;

		WindowProperties(const char* _title = "Untitled Window") : title(_title) {}
	};

	class Window {
	public:
		virtual ~Window(){};

		const glm::vec2 GetWindowPosition() const;
		const uint32_t GetXPosition() const;
		const uint32_t GetYPosition() const;

		const glm::ivec2 GetSize() const; 
		void SetSize(const glm::ivec2& size);
		void SetSize(const uint32_t& x, const uint32_t& y);

		inline SDL_Window* GetNativeWindow() const{ return window; }

		static Window* Create(const WindowProperties& properties = WindowProperties());
	protected:
		WindowProperties properties;
		SDL_Window* window;
	};
}