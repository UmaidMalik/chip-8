#pragma once

#include <fmt/core.h>
#include <fmt/printf.h>
#include <string_view>
#include <SDL3/SDL.h>

enum class LogLevel
{
    Default,
    Info,
    Warn,
    Error,
    Debug
};

class Logger
{
    private:
        const char* prefix(LogLevel lvl)
        {
            switch(lvl)
            {
                case LogLevel::Default: return "";
                case LogLevel::Info:    return "\033[32m[INFO] \033[0m";
                case LogLevel::Warn:    return "\033[33m[WARN] \033[0m ";
                case LogLevel::Error:   return "\033[31m[ERROR] \033[0m";
                case LogLevel::Debug:   return "\033[34m[DEBUG] \033[0m";
                default:                return "";
            }
            return "";
        }
    public:
        Logger() = default;
        
        template<typename... Args>
        void Log(LogLevel lvl, fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            fmt::print(stderr, "{}{}", prefix(lvl), fmt::format(fmt_str, std::forward<Args>(args)...));
        }

        template<typename... Args>
        void Print(fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            Log(LogLevel::Default, fmt_str, std::forward<Args>(args)...);
        }
        
        template<typename... Args>
        void Info(fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            Log(LogLevel::Info, fmt_str, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Warn(fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            Log(LogLevel::Warn, fmt_str, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Error(fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            Log(LogLevel::Error, fmt_str, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Debug(fmt::format_string<Args...> fmt_str, Args&&... args)
        {
            Log(LogLevel::Debug, fmt_str, std::forward<Args>(args)...);
        }
};