#pragma once
#include <spdlog/spdlog.h>

namespace mist {
    class Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetApplicationLogger() { return applicationLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }
    private:
        static std::shared_ptr<spdlog::logger> logger;
        static std::shared_ptr<spdlog::logger> applicationLogger;
    };
}

// TODO: Add log stripping

// App Logging
#define APP_TRACE(...)			mist::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)			mist::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define APP_WARN(...)			mist::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)			mist::Log::GetApplicationLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)		mist::Log::GetApplicationLogger()->critical(__VA_ARGS__)

// Engine Logging
#define MIST_TRACE(...)			mist::Log::GetLogger()->trace(__VA_ARGS__)
#define MIST_INFO(...)			mist::Log::GetLogger()->info(__VA_ARGS__)
#define MIST_WARN(...)			mist::Log::GetLogger()->warn(__VA_ARGS__)
#define MIST_ERROR(...)			mist::Log::GetLogger()->error(__VA_ARGS__)
#define MIST_CRITICAL(...)		mist::Log::GetLogger()->critical(__VA_ARGS__)