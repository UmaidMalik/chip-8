#pragma once

#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <string>
#include <SDL3/SDL.h>

enum class LogLevel
{
    Info,
    Warn,
    Error,
    Debug
};

class Logger
{
    private:
        void vlog(LogLevel lvl, const char* fmt, va_list args)
        {
            const char* prefix = nullptr;
            switch(lvl)
            {
                case LogLevel::Info:    prefix = "[INFO] "; break;
                case LogLevel::Warn:    prefix = "[WARN] "; break;
                case LogLevel::Error:   prefix = "[ERROR] "; break;
                case LogLevel::Debug:   prefix = "[DEBUG] "; break;
            }
            std::fprintf(stderr, "%s", prefix);
            std::vfprintf(stderr, fmt, args);
            std::fprintf(stderr, "\n");           
        }
    public:
        explicit Logger() {};
        void Log(LogLevel lvl, const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vlog(lvl, fmt, args);
            va_end(args);
        }
        
        void Info(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vlog(LogLevel::Info, fmt, args);
            va_end(args);
        }

        void Warn(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vlog(LogLevel::Warn, fmt, args);
            va_end(args);
        }

        void Error(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vlog(LogLevel::Error, fmt, args);
            va_end(args);
        }

        void Debug(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vlog(LogLevel::Debug, fmt, args);
            va_end(args);
        }
};