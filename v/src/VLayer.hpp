#pragma once
#include <imgui/ImguiLayer.hpp>
#include "Game.hpp"

namespace V {
	class VLayer : public mist::ImguiLayer {
	public:
		VLayer(const mist::ImguiLayerConfig& config);
		virtual ~VLayer() override;

		VLayer(const VLayer& other) = delete;
		VLayer& operator=(const VLayer& other) = delete;
	
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnRender() override;
	private:
		void OnImguiRender();
		void PostRender();

		bool resizeRequested = false;
		glm::ivec2 windowSize = { 0, 0 };
		Game game;
	};
}