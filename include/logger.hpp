#pragma once

#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <string>
#include <SDL3/SDL.h>

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

enum class LogSink
{
    SDL,
    StdErr,
    StdOut
};

class Logger
{
    private:
        std::mutex m_mutex;
        LogSink m_sink;

        void vlog(LogLevel lvl, const char* fmt, va_list args)
        {  
            switch (m_sink)
            {
                case LogSink::SDL:
                #if defined(SDL_VERSION_ATLEAST)
                    {
                        int priority = 0;
                        switch(lvl)
                        {
                            case LogLevel::Trace: case LogLevel::Debug: priority = 1; break;
                            case LogLevel::Info: priority = 2; break;
                            case LogLevel::Warn: priority = 3; break;
                            case LogLevel::Error: priority = 4; break;
                            case LogLevel::Critical: priority = 5; break;
                        }
                        extern void SDL_LogMessageV(int category, int priority, const char* fmt, va_list ap);
                        SDL_LogMessageV(0, priority, fmt, args);
                        break;
                    }   
                #endif
                    break;
                case LogSink::StdErr:
                    std::vfprintf(stderr, fmt, args);
                    std::fputc('\n', stderr);
                    std::fflush(stderr);
                    break;
                case LogSink::StdOut:
                {
                    char buffer[2048];
                    vsnprintf(buffer, sizeof(buffer), fmt, args);
                    fprintf(stdout, "%s\n", buffer);
                    fflush(stdout);
                    break;
                }
            }
        }
    public:
        explicit Logger(LogSink sink = LogSink::SDL) : m_sink(sink) {};
        void SetSink(LogSink s) { m_sink = s; };
        void Log(LogLevel lvl, const char* fmt, ...)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            va_list args;
            va_start(args, fmt);
            va_end(args);
        }
        
        void Info(const char* fmt, ...)
        {
            va_list a;
            va_start(a, fmt);
            vlog(LogLevel::Info, fmt, a);
            va_end(a);
        }

        void Warn(const char* fmt, ...)
        {
            va_list a;
            va_start(a, fmt);
            vlog(LogLevel::Warn, fmt, a);
            va_end(a);
        }

        void Error(const char* fmt, ...)
        {
            va_list a;
            va_start(a, fmt);
            vlog(LogLevel::Error, fmt, a);
            va_end(a);
        }
};