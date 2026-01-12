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

#include <gtest/gtest.h>

#include "logger/Logger.h"

#include <fstream>
#include <cstdio>

class LoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        logger = &Logger::GetInstance();
        logger->SetLevel(Logger::TRACE);
        logger->EnableConsoleOutput(false);
        testLogFile = "/tmp/opentpg_test.log";
    }

    void TearDown() override
    {
        logger->EnableConsoleOutput(true);
        logger->SetLevel(Logger::INFO);
        std::remove(testLogFile.c_str());
    }

    Logger* logger;
    std::string testLogFile;
};

TEST_F(LoggerTest, GetInstance)
{
    ASSERT_NE(logger, nullptr);
    Logger& logger2 = Logger::GetInstance();
    EXPECT_EQ(logger, &logger2);
}

TEST_F(LoggerTest, SetLevel)
{
    logger->SetLevel(Logger::ERROR);
    logger->SetOutputFile(testLogFile);
    LOG_WARN("This should not be logged");
    LOG_ERROR("This should be logged");

    std::ifstream file(testLogFile);
    std::string line;
    bool foundError = false;
    bool foundWarn = false;
    while (std::getline(file, line)) {
        if (line.find("ERROR") != std::string::npos) {
            foundError = true;
        }
        if (line.find("WARN") != std::string::npos) {
            foundWarn = true;
        }
    }
    file.close();

    EXPECT_TRUE(foundError);
    EXPECT_FALSE(foundWarn);
}

TEST_F(LoggerTest, LogLevels)
{
    logger->SetOutputFile(testLogFile);

    LOG_TRACE("Trace message");
    LOG_DEBUG("Debug message");
    LOG_INFO("Info message");
    LOG_WARN("Warning message");
    LOG_ERROR("Error message");
    LOG_FATAL("Fatal message");

    std::ifstream file(testLogFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    EXPECT_TRUE(content.find("TRACE") != std::string::npos);
    EXPECT_TRUE(content.find("DEBUG") != std::string::npos);
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
    EXPECT_TRUE(content.find("WARN") != std::string::npos);
    EXPECT_TRUE(content.find("ERROR") != std::string::npos);
    EXPECT_TRUE(content.find("FATAL") != std::string::npos);
}

TEST_F(LoggerTest, LogFormat)
{
    logger->SetOutputFile(testLogFile);

    int value = 42;
    LOG_INFO("Test value: %d", value);

    std::ifstream file(testLogFile);
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line.find("42") != std::string::npos) {
            found = true;
            break;
        }
    }
    file.close();

    EXPECT_TRUE(found);
}

TEST_F(LoggerTest, ConsoleOutputToggle)
{
    logger->EnableConsoleOutput(true);
    LOG_INFO("Console message");
    logger->EnableConsoleOutput(false);
    LOG_INFO("No console message");
}
