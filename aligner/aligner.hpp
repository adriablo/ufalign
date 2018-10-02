#pragma once

#include <cassert>
#include <thread>

#include "logger.hpp"
#include "index_cfg.hpp"
#include "cl_opt.hpp"
#include "handler.hpp"
#include "cache.hpp"


struct TTargetAlign {
    TMyFMIndex &myIndex;
    CharString &targetId;
    Dna5String &targetSeq;
    std::ofstream &outFile;
};
