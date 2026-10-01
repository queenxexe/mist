#include "platform/linux/LinuxWindow.hpp"
#include <vulkan/vulkan.h>
#include "Application.hpp"

namespace mist {
	LinuxWindow::LinuxWindow(const WindowProperties& properties) {
		this->properties = properties;
		SDL_WindowFlags flags = 0;
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
			case RenderAPI::API::Vulkan:
				flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
				break;
			default:
				break;
		}

		window = SDL_CreateWindow(
			properties.title,
			INIT_WIDTH,
			INIT_HEIGHT,
			flags
		);
	}

	LinuxWindow::~LinuxWindow() {
		SDL_DestroyWindow(window);
	}
}