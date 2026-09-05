#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#endif

#include <Application.hpp>
#include "VLayer.hpp"

int main(int arc, char* argv[]) {
	mist::Application v = mist::Application("V");

	mist::ImguiLayerConfig config;
    config.name = "V";
    config.fontPath = "";
    config.flags = ImGuiConfigFlags_NavEnableKeyboard;
	V::VLayer* gameLayer = new V::VLayer(config);

    v.PushLayer(gameLayer);
	v.Run();
}