#pragma once

//
// https://github.com/gabime/spdlog
//

// Uncomment SPDLOG_DEBUG_ON/SPDLOG_TRACE_ON in tweakme.h to enable.
// SPDLOG_TRACE(..) will also print current file and line.

#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

typedef std::shared_ptr<spdlog::logger> TLogger;

extern TLogger logger;


void start_logger(const char *name,
                  spdlog::level::level_enum consoleLevel = spdlog::level::info,
                  spdlog::level::level_enum fileLevel = spdlog::level::debug
                  );
