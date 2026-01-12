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
#ifndef OPENTPG_LOGGER_H
#define OPENTPG_LOGGER_H

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

class Logger
{
public:
    enum Level {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    /**
     * 获取 Logger 单例实例
     * @return Logger 单例引用
     */
    static Logger& GetInstance();

    /**
     * 设置日志输出级别
     * 只有大于等于此级别的日志才会被输出
     * @param level 日志级别
     */
    void SetLevel(Level level);

    /**
     * 设置日志输出文件
     * @param filename 日志文件路径
     */
    void SetOutputFile(const std::string& filename);

    /**
     * 启用或禁用控制台输出
     * @param enable true 启用，false 禁用
     */
    void EnableConsoleOutput(bool enable);

    /**
     * 记录日志
     * @param level 日志级别
     * @param file 源文件名
     * @param line 行号
     * @param format 格式化字符串
     * @param ... 可变参数
     */
    void Log(Level level, const char* file, int line, const char* format, ...);

    /**
     * 刷新日志缓冲区
     * 将所有缓冲的日志立即写入文件和控制台
     */
    void Flush();

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * 写入日志到输出目标
     * @param level 日志级别
     * @param message 日志消息
     */
    void WriteLog(Level level, const std::string& message);

    /**
     * 将日志级别转换为字符串
     * @param level 日志级别
     * @return 级别字符串
     */
    const char* LevelToString(Level level);

    /**
     * 格式化当前时间
     * @return 格式化后的时间字符串 (YYYY-MM-DD HH:MM:SS)
     */
    std::string FormatTime();

private:
    Level currentLevel;
    bool consoleOutput;
    std::ofstream fileStream;
    std::mutex logMutex;
    std::string logFilePath;
};

#define LOG_TRACE(fmt, ...) Logger::GetInstance().Log(Logger::TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::GetInstance().Log(Logger::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Logger::GetInstance().Log(Logger::INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Logger::GetInstance().Log(Logger::WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::GetInstance().Log(Logger::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::GetInstance().Log(Logger::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif //OPENTPG_LOGGER_H
