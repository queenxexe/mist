#if __linux__
#include "Utils.hpp"
#include <cstdlib>
#include "Debug.hpp"

namespace mist {
	bool IsCommandAvailable(const std::string& command) {
		std::string checkCommand = "command -v " + command + " >/dev/null 2>&1";
		int result = system(checkCommand.c_str());
		return result == 0;
	}

	std::string OpenDialog(const std::string& command) {
		std::array<char, 128> buffer;
		std::string result;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) {
			return std::string();
			return std::string();
		}
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}

		if (!result.empty() && result.back() == '\n') {
			result.pop_back();
		}

		return result;
	}

	std::string FileDialog::OpenFile(const std::string& filter) {
		if (IsCommandAvailable("zenity")) {
			std::string zenityCommand("zenity --file-selection --title='Open File' --file-filter='" + filter + "'");
			return OpenDialog(zenityCommand);
		} else if (IsCommandAvailable("kdialog")) {
			std::string kdialogCommand("kdialog --getopenfilename . \"" + filter + "\"");
			return OpenDialog(kdialogCommand);
		}
		
		MIST_WARN("Cant open file dialog, please install zenity or kdialog.");
		return "";
	}
	
	std::string FileDialog::SaveFile(const std::string& filter) {
		if (IsCommandAvailable("zenity")) {
			std::string zenityCommand("zenity --file-selection --save --confirm-overwrite --title='Save File' --file-filter='" + filter + "'");
			return OpenDialog(zenityCommand);
		} else if (IsCommandAvailable("kdialog")) {
			std::string kdialogCommand("kdialog --getsavefilename . \"" + filter + "\"");
			return OpenDialog(kdialogCommand);
		}
		
		MIST_WARN("Cant open save dialog, please install zenity or kdialog.");
		return "";
	}
}
#endif