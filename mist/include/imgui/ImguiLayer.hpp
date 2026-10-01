#pragma once
#include "Layer.hpp"
#include <imgui.h>
#include "renderer/Framebuffer.hpp"

namespace mist {
	struct ImguiLayerConfig {
		const char* name;		// Layer name 
		const char* fontPath;	// Initial imgui font on creation if left empty will use imgui default
		ImGuiConfigFlags flags;	// Flags to setup Imgui IO with
	};

	class ImguiLayer : public Layer {
	public:
		ImguiLayer(const ImguiLayerConfig& config);
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(const SDL_Event* e) override;

		void Begin();
		void End();
		ImTextureID AddTexture(const std::shared_ptr<RenderData>& renderData);
		void UpdateTexture(ImTextureID& id, const std::shared_ptr<RenderData>& renderData);
		void RemoveTexture(const ImTextureID& id);
	protected:
		void SetDarkThemeColors();

		ImguiLayerConfig config;
		std::shared_ptr<RenderData> renderData;
	};
}