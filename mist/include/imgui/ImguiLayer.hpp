#pragma once
#include "Layer.hpp"
#include <imgui.h>
#include "renderer/Framebuffer.hpp"

namespace mist {
	class ImguiLayer : public Layer {
	public:
		ImguiLayer(const char* name = "Imgui Layer");
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(const SDL_Event* e) override;

		void Begin();
		void End();
		ImTextureID AddTexture(const Ref<RenderData>& renderData);
		void UpdateTexture(ImTextureID& id, const Ref<RenderData>& renderData);
		void RemoveTexture(const ImTextureID& id);
		void ImGuiImage(const ImTextureRef& texture, const ImVec2& imageSize, const ImVec2& uv0, const ImVec2& uv1);
	protected:
		void SetDarkThemeColors();

		Ref<RenderData> renderData;
	};
}