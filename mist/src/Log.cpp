#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mist {
    std::shared_ptr<spdlog::logger> Log::logger;
    std::shared_ptr<spdlog::logger> Log::applicationLogger;

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
		logger = spdlog::stdout_color_mt("mist");
		logger->set_level(spdlog::level::trace);

		applicationLogger = spdlog::stdout_color_mt("app");
		applicationLogger->set_level(spdlog::level::trace);
    }
}