/**
 * Copyright (c) 2025 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include "Logger.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : currentLevel(INFO)
    , consoleOutput(true)
{
}

Logger::~Logger()
{
    Flush();
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Logger::SetLevel(Level level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    currentLevel = level;
}

void Logger::SetOutputFile(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (fileStream.is_open()) {
        fileStream.close();
    }
    logFilePath = filename;
    fileStream.open(filename, std::ios::out | std::ios::app);
}

void Logger::EnableConsoleOutput(bool enable)
{
    std::lock_guard<std::mutex> lock(logMutex);
    consoleOutput = enable;
}

void Logger::Log(Level level, const char* file, int line, const char* format, ...)
{
    if (level < currentLevel) {
        return;
    }

    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::ostringstream oss;
    oss << "[" << FormatTime() << "] "
        << "[" << LevelToString(level) << "] "
        << "[" << file << ":" << line << "] "
        << buffer;

    WriteLog(level, oss.str());
}

void Logger::Flush()
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (fileStream.is_open()) {
        fileStream.flush();
    }
    std::cout.flush();
    std::cerr.flush();
}

void Logger::WriteLog(Level level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (consoleOutput) {
        if (level >= ERROR) {
            std::cerr << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }

    if (fileStream.is_open()) {
        fileStream << message << std::endl;
    }
}

const char* Logger::LevelToString(Level level)
{
    switch (level) {
    case TRACE: return "TRACE";
    case DEBUG: return "DEBUG";
    case INFO:  return "INFO ";
    case WARN:  return "WARN ";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    default:    return "UNKN ";
    }
}

std::string Logger::FormatTime()
{
    time_t now = time(nullptr);
    struct tm* ttm = localtime(&now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ttm);
    return std::string(buffer);
}
