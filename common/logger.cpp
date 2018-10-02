
#include "logger.hpp"


/*
https://github.com/gabime/spdlog/wiki/2.-Creating-loggers
'Creating loggers with multiple sinks'
'example/multisink.cpp'
*/
void start_logger(const char *name,
                  spdlog::level::level_enum consoleLevel,
                  spdlog::level::level_enum fileLevel) {

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(consoleLevel);
    // console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    std::string fname = std::string(name) + ".log";
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fname, true);
    file_sink->set_level(fileLevel);

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(console_sink);
    sinks.push_back(file_sink);

    auto my_log = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
    spdlog::register_logger(my_log);

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    // spdlog::level::level_enum min_level = std::min(consoleLevel, fileLevel);
    // spdlog::set_level(min_level);
    // my_log->set_level(min_level);

    // my_log->flush_on(std::max(consoleLevel, fileLevel));
}
