#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#endif

#include <Application.hpp>
#include "EditorLayer.hpp"

// TODO: fix cmake issues with editor make it just use main() and not shitty winmain()
// i cba rn to fix this shit

// Now its getting even more spec with winmains

void Run() {
	mist::Application editor = mist::Application("Editor");
	
	mist::ImguiLayerConfig config;
	config.name = "Editor";
	config.fontPath = "assets/fonts/Roboto/Roboto-Bold.ttf";
	config.flags = 
		ImGuiConfigFlags_NavEnableKeyboard |
		ImGuiConfigFlags_DockingEnable |
		ImGuiConfigFlags_ViewportsEnable;
	mistEditor::EditorLayer* editorLayer = new mistEditor::EditorLayer(config);

	editor.PushLayer(editorLayer);
	editor.Run();
}

//#if defined(_WIN32)
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
//    Run();
//    return 0;
//}

//int WinMain() {
//    Run();
//    return 0;
//}
//#else
int main(int argc, char* argv[]) {
	Run();
	return 0;
}
//#endif