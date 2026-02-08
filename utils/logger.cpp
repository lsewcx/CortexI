#include "logger.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>

#if __has_include(<glog/logging.h>)
  #include <glog/logging.h>
  #define CORTEXI_HAS_GLOG 1
#else
  #define CORTEXI_HAS_GLOG 0
#endif

namespace cortexi {

static bool g_inited = false;
static bool g_fatal_terminates = true;

LogOptions LogOptions::FromJson(const std::string& json_path) {
  LogOptions opts;
  std::ifstream ifs(json_path);
  if (!ifs.is_open()) {
    std::cerr << "Failed to open logger config file: " << json_path << std::endl;
    return opts;
  }
  try {
    json j = json::parse(ifs);
    if (j.contains("app_name")) opts.app_name = j["app_name"].get<std::string>();
    if (j.contains("log_dir")) opts.log_dir = j["log_dir"].get<std::string>();
    if (j.contains("also_to_stderr")) opts.also_to_stderr = j["also_to_stderr"].get<bool>();
    if (j.contains("stderr_threshold")) opts.stderr_threshold = j["stderr_threshold"].get<int>();
    if (j.contains("vlog")) opts.vlog = j["vlog"].get<int>();
    if (j.contains("log_prefix")) opts.log_prefix = j["log_prefix"].get<bool>();
    if (j.contains("color")) opts.color = j["color"].get<bool>();
    if (j.contains("fatal_terminates")) opts.fatal_terminates = j["fatal_terminates"].get<bool>();
    if (j.contains("use_custom_sink")) opts.use_custom_sink = j["use_custom_sink"].get<bool>();
  } catch (const std::exception& e) {
    std::cerr << "Failed to parse logger config: " << e.what() << std::endl;
  }
  return opts;
}

void Logging::Init(const std::string& json_path) {
  LogOptions opts = LogOptions::FromJson(json_path);
  Init(opts);
}

void Logging::Init(const LogOptions& opts) {
#if CORTEXI_HAS_GLOG
  if (g_inited) return;
  g_fatal_terminates = opts.fatal_terminates;
  google::InitGoogleLogging(opts.app_name.c_str());
  if (!opts.log_dir.empty()) {
    google::SetLogDestination(google::GLOG_INFO,    (opts.log_dir + "/info_").c_str());
    google::SetLogDestination(google::GLOG_WARNING, (opts.log_dir + "/warn_").c_str());
    google::SetLogDestination(google::GLOG_ERROR,   (opts.log_dir + "/error_").c_str());
    google::SetLogDestination(google::GLOG_FATAL,   (opts.log_dir + "/fatal_").c_str());
  }
  if (opts.also_to_stderr) {
    int sev_i = google::GLOG_INFO + opts.stderr_threshold;
    if (sev_i < google::GLOG_INFO) sev_i = google::GLOG_INFO;
    if (sev_i > google::GLOG_FATAL) sev_i = google::GLOG_FATAL;
    google::SetStderrLogging(static_cast<google::LogSeverity>(sev_i));
  }
  if (opts.vlog > 0) {
    google::SetVLOGLevel("*", opts.vlog);
  }
  g_inited = true;
#else
  (void)opts;
  g_inited = true;
#endif
}

void Logging::Shutdown() {
#if CORTEXI_HAS_GLOG
  if (g_inited) google::ShutdownGoogleLogging();
#endif
  g_inited = false;
}

void Logging::Info(const std::string& msg) {
#if CORTEXI_HAS_GLOG
  LOG(INFO) << msg;
#else
  std::cerr << "[INFO] " << msg << std::endl;
#endif
}

void Logging::Warn(const std::string& msg) {
#if CORTEXI_HAS_GLOG
  LOG(WARNING) << msg;
#else
  std::cerr << "[WARN] " << msg << std::endl;
#endif
}

void Logging::Error(const std::string& msg) {
#if CORTEXI_HAS_GLOG
  LOG(ERROR) << msg;
#else
  std::cerr << "[ERROR] " << msg << std::endl;
#endif
}

void Logging::Fatal(const std::string& msg) {
#if CORTEXI_HAS_GLOG
  if (g_fatal_terminates) {
    LOG(FATAL) << msg;
  } else {
    LOG(ERROR) << "[FATAL] " << msg;
  }
#else
  std::cerr << "[FATAL] " << msg << std::endl;
  if (g_fatal_terminates) {
    std::abort();
  }
#endif
}

} // namespace cortexi