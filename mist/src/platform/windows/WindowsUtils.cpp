#if _WIN32
#include "Utils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdlg.h>
#include <SDL3/SDL.h>
#include "Application.hpp"

namespace mist {
	static HWND GetWindowsWindow() {
		return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(Application::Get().GetWindow()->GetNativeWindow()), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	}

	std::string FileDialog::OpenFile(const char* filter) {
		OPENFILENAMEA ofn;	// Common dialog box structure
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetWindowsWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialog::SaveFile(const char* filter) {
		OPENFILENAMEA ofn;	// Common dialog box structure
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetWindowsWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}
		return std::string();
	}
}
#endif