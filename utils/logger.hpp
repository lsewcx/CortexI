#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace cortexi {

using json = nlohmann::json;

struct LogOptions {
  std::string app_name = "CortexI";
  std::string log_dir = "";
  bool also_to_stderr = true;
  int stderr_threshold = 1;
  int vlog = 0;
  bool fatal_terminates = true;

  static LogOptions FromJson(const std::string& json_path);
};

class Logging {
public:
  static void Init(const LogOptions& opts);
  static void Init(const std::string& json_path);
  static void Shutdown();
  static void Info(const std::string& msg);
  static void Warn(const std::string& msg);
  static void Error(const std::string& msg);
  static void Fatal(const std::string& msg);
};

class LoggingGuard {
public:
  LoggingGuard(const LogOptions& opts) {
    Logging::Init(opts);
  }
  LoggingGuard(const std::string& json_path) {
    Logging::Init(json_path);
  }
  ~LoggingGuard() {
    Logging::Shutdown();
  }
};

} // namespace cortexi