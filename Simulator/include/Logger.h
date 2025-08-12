#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <cxxabi.h>
#ifdef ENABLE_LOGGING
    #define LOG(level, msg) Logger::instance().log(level, msg)
#else
    #define LOG(level, msg) ((void)0)
#endif
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
private:
    Logger() = default;
    ~Logger() = default;


    std::mutex mtx;

    static const char* toString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    static std::string demangle(const char* mangled_name) {
        int status = 0;
        std::unique_ptr<char, void(*)(void*)> res(
                abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status),
                std::free);

        return (status == 0) ? res.get() : mangled_name;
    }

    void log(LogLevel level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[" << toString(level) << "] " << msg << std::endl;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

};