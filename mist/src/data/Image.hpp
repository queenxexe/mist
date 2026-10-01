#pragma once
#include "renderer/RenderTypes.hpp"

namespace mist {
	struct Image {
	public:
		virtual ~Image() {};
		
		virtual void Cleanup() = 0;
		
		ImageData GetImageData() { return data; }
	protected:
		ImageData data;
	};
}