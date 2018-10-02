
#include <seqan/index.h>
#include <seqan/seq_io.h>

#include "index_cfg.hpp"
#include "logger.hpp"

using namespace seqan;

static const char *target_path;

const char *logName = "aligner";
TLogger logger;


void start_the_logger() {
    const char *logName = "indexer";
    start_logger(logName);
    logger = spdlog::get(logName);
    logger->trace("we have the logger!");
    SPDLOG_TRACE(logger, "SPDLOG_TRACE");
}


int parse_args(int argc, char* const* argv) {
    extern char *optarg;
    int flag = 0;
    int c;
    while ((c = getopt (argc, argv, "t:")) != -1) {
        switch (c) {
        case 't':
            target_path = optarg;
            flag++;
            break;
        default:
            // bad!
            flag = -1;
            break;
        }        
    }
    if(flag < 1) {
        printf("Usage: %s\n", "t:i:q:d:");
        logger->error("Bad arguments");
        return -1;
    }
    return 0;
}


int main(int argc, char* const* argv)
{
    start_the_logger();

    int res = parse_args(argc, argv);
    if(res != 0) {
        // logger->error("Bad arguments");
        return res;
    }

    SeqFileIn targetFileIn(target_path);
    CharString targetId;
    Dna5String targetSeq;
    // parse the target sequences
    while (! atEnd(targetFileIn)) {
        readRecord(targetId, targetSeq, targetFileIn);
        size_t targetLen = length(targetSeq);
        logger->info("Completed: target load: {}", toCString(targetId));

        TMyFMIndex myIndex(targetSeq);

        // create the index from sequence and save to disk
        indexRequire(myIndex, FibreSA());
        logger->info("Completed: indexRequire");
        save(myIndex, toCString(targetId));
        logger->info("Completed: FMIndex save");
    }

    return 0;
}
