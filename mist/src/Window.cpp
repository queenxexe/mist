#include "Window.hpp"

#if _WIN32
#include "platform/windows/WindowsWindow.hpp"
#elif __linux__
#include "platform/linux/LinuxWindow.hpp"
#endif

namespace mist {
	Window* Window::Create(const WindowProperties& properties) {
#if _WIN32
		return new WindowsWindow(properties);
#elif __linux__
		return new LinuxWindow(properties);
#endif
	}

	const glm::vec2 Window::GetWindowPosition() const {
		int x,y;
		SDL_GetWindowPosition(window, &x, &y);
		return glm::vec2 { x, y };	
	};

	const uint32_t Window::GetXPosition() const { return (uint32_t)GetWindowPosition().x; };
	const uint32_t Window::GetYPosition() const { return (uint32_t)GetWindowPosition().y; };

	const glm::ivec2 Window::GetSize() const {
		glm::ivec2 size{0,0};
		SDL_GetWindowSize(window, &size.x, &size.y);
		return size;
	}

	void Window::SetSize(const glm::ivec2& size) {
		SetSize(size.x, size.y);
	}

	void Window::SetSize(const uint32_t& x, const uint32_t& y) {
		SDL_SetWindowSize(window, x, y);
	}
}