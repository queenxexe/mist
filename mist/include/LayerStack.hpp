#pragma once
#include "Layer.hpp"

namespace mist {
	class LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline Layer** begin() { return layers; }		// Lowercase to allow for(Layer* layer : layerstack) {}
		inline Layer** end() { return layers + size; }	// Lowercase to allow for(Layer* layer : layerstack) {}
		inline uint32_t Size() { return size; }
		
		void Clear();
		void Resize(uint32_t newCapacity);

		Layer** layers;
	private:
		uint32_t capacity;
		uint32_t size;
	};
}