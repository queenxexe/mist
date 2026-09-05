#pragma once
#include <imgui/ImguiLayer.hpp>

namespace V {
	class VLayer : public mist::ImguiLayer {
	public:
		VLayer(const mist::ImguiLayerConfig& config);
		virtual ~VLayer() override;

		VLayer(const VLayer& other) = delete;
		VLayer& operator=(const VLayer& other) = delete;
	
		virtual void OnUpdate() override;
		virtual void OnRender() override;
	private:
		void OnImguiRender();
	};
}