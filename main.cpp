#include "utils/logger.hpp"

int main()
{
    cortexi::LoggingGuard guard("../configs/logger_config.json");
    cortexi::Logging::Info("Hello, World!");
    cortexi::Logging::Warn("This is a warning message.");
    cortexi::Logging::Error("This is an error message.");
    cortexi::Logging::Fatal("This is a fatal message.");
    return 0;
}