#include <gtest/gtest.h>
#include "utils/logger.hpp"
#include <fstream>
#include <thread>
#include <chrono>

namespace cortexi {

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    test_config_path_ = "test_logger_config.json";
    CreateTestConfig();
  }

  void TearDown() override {
    std::remove(test_config_path_.c_str());
  }

  void CreateTestConfig() {
    std::ofstream ofs(test_config_path_);
    ofs << R"({
  "app_name": "CortexITest",
  "log_dir": "",
  "also_to_stderr": true,
  "stderr_threshold": 0,
  "vlog": 0,
  "log_prefix": false,
  "color": false,
  "fatal_terminates": false,
  "use_custom_sink": false
})";
  }

  std::string test_config_path_;
};

TEST_F(LoggerTest, InitWithJsonPath) {
  LoggingGuard guard(test_config_path_);
  EXPECT_NO_THROW({
    Logging::Info("Test info message");
  });
}

TEST_F(LoggerTest, InitWithLogOptions) {
  LogOptions opts;
  opts.app_name = "TestApp";
  opts.stderr_threshold = 0;
  opts.fatal_terminates = false;
  
  LoggingGuard guard(opts);
  EXPECT_NO_THROW({
    Logging::Info("Test info message");
  });
}

TEST_F(LoggerTest, LogOptionsFromJson) {
  LogOptions opts = LogOptions::FromJson(test_config_path_);
  
  EXPECT_EQ(opts.app_name, "CortexITest");
  EXPECT_EQ(opts.also_to_stderr, true);
  EXPECT_EQ(opts.stderr_threshold, 0);
  EXPECT_EQ(opts.fatal_terminates, false);
}

TEST_F(LoggerTest, LogOptionsFromJsonInvalidPath) {
  LogOptions opts = LogOptions::FromJson("non_existent_file.json");
  
  EXPECT_EQ(opts.app_name, "CortexI");
  EXPECT_EQ(opts.also_to_stderr, true);
}

TEST_F(LoggerTest, LogInfo) {
  LoggingGuard guard(test_config_path_);
  EXPECT_NO_THROW({
    Logging::Info("Info message");
  });
}

TEST_F(LoggerTest, LogWarn) {
  LoggingGuard guard(test_config_path_);
  EXPECT_NO_THROW({
    Logging::Warn("Warning message");
  });
}

TEST_F(LoggerTest, LogError) {
  LoggingGuard guard(test_config_path_);
  EXPECT_NO_THROW({
    Logging::Error("Error message");
  });
}

TEST_F(LoggerTest, LogFatalNoTerminate) {
  LogOptions opts = LogOptions::FromJson(test_config_path_);
  opts.fatal_terminates = false;
  
  LoggingGuard guard(opts);
  EXPECT_NO_THROW({
    Logging::Fatal("Fatal message");
  });
}

TEST_F(LoggerTest, LogFatalWithTerminate) {
  LogOptions opts = LogOptions::FromJson(test_config_path_);
  opts.fatal_terminates = true;
  
  LoggingGuard guard(opts);
  EXPECT_DEATH({
    Logging::Fatal("Fatal message");
  }, "");
}

TEST_F(LoggerTest, MultipleLogLevels) {
  LoggingGuard guard(test_config_path_);
  
  EXPECT_NO_THROW({
    Logging::Info("Info");
    Logging::Warn("Warning");
    Logging::Error("Error");
    Logging::Fatal("Fatal");
  });
}

TEST_F(LoggerTest, Shutdown) {
  Logging::Init(test_config_path_);
  EXPECT_NO_THROW({
    Logging::Info("Before shutdown");
  });
  
  Logging::Shutdown();
  
  EXPECT_NO_THROW({
    Logging::Info("After shutdown");
  });
}

TEST_F(LoggerTest, LoggingGuardAutoCleanup) {
  {
    LoggingGuard guard(test_config_path_);
    EXPECT_NO_THROW({
      Logging::Info("Inside guard scope");
    });
  }
  
  EXPECT_NO_THROW({
    Logging::Info("Outside guard scope");
  });
}

TEST_F(LoggerTest, StderrThresholdFiltering) {
  LogOptions opts = LogOptions::FromJson(test_config_path_);
  opts.stderr_threshold = 2;
  
  LoggingGuard guard(opts);
  
  EXPECT_NO_THROW({
    Logging::Info("Info message");
    Logging::Warn("Warning message");
    Logging::Error("Error message");
  });
}

} // namespace cortexi

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}