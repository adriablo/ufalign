
#include <cassert>
#include <thread>

#include "aligner.hpp"


using namespace seqan;

const char *logName = "aligner";
TLogger logger;


TLogger start_the_logger() {
    start_logger(logName, spdlog::level::info, spdlog::level::debug);
    logger = spdlog::get(logName);
    // logger->trace("we have the logger!");
    // SPDLOG_TRACE(logger, "SPDLOG_TRACE");
    logger->set_level(spdlog::level::warn);
    logger->flush_on(spdlog::level::info);
    return logger;
}


void stop_the_logger() {
    spdlog::drop(logName);
}


void align_to_target(TCLOpt &opt, TTargetAlign &tal, unsigned int batch, unsigned int batchCount) {
    // enumerate the query sequences
    size_t targetLen = length(tal.targetSeq);
    TMyFinder myFinder(tal.myIndex);

    SeqFileIn queryFileIn(opt.queryPath);
    int lineNo = -1;
    logger->debug("batch: {}, batchCount: {}", batch, batchCount);
    while (! atEnd(queryFileIn)) {
        //
        CharString queryId;
        Dna5String querySeq;
        readRecord(queryId, querySeq, queryFileIn);
        size_t queryLen = length(querySeq);
        logger->debug("readRecord(queryId): '{}', len: {}", toCString(queryId), queryLen);
        // test batch
        lineNo ++;
        if (lineNo % batchCount != batch) {
            continue;
        }
        logger->debug("batch: {}, handle lineNo: {}", batch, lineNo);

        int fragLen = queryLen / (opt.editDist + 1);
        logger->debug("fragLen: {}", fragLen);

        TCSString css_query = querySeq;
        const char *c_query = toCString(css_query);

        THitCache hitCache;

        // sweep the query and find occurrences of fragment in target
        // need to use uint (32 bit) because the targetLen can go up to a billion (10^9),
        // and also position(myFinder) is unsigned (int)
        // TODO: there are a lot of redundant hits as we slide the pattern one position
        // at a time, over the query
        // 'fragment' and 'pattern' are just different names for the same concept
        for(int fragStart = 0; fragStart < queryLen - fragLen + 1; fragStart++) {
            // query Prefix and Suffix (before and after the pattern fragment)
            uint prefixLen = fragStart;
            assert(queryLen >= prefixLen + fragLen);
            uint suffixLen = queryLen - prefixLen - fragLen;
            assert(prefixLen + fragLen + suffixLen == queryLen);
            // max extensions around the pattern position (hit), to read from target
            uint leftExt = prefixLen + opt.editDist;
            uint rightExt = fragLen + suffixLen + opt.editDist;
            assert(leftExt + rightExt == queryLen + 2 * opt.editDist);

            TMyInfix queryInfix = infix(querySeq, fragStart, fragStart + fragLen);

            Dna5String pattern;
            append(pattern, queryInfix);
            TCSString css_pattern = pattern;
            const char *c_pattern = toCString(css_pattern);
            logger->debug("pattern: {}", c_pattern);
            // reset/reuse Finder
            goBegin(myFinder);
            clear(myFinder);
            while (find(myFinder, pattern))
            {
                uint pos = position(myFinder);
                // logger->debug("myFinder position: {}", pos);
                // read maximum possible extent around the position
                uint startPos, endPos;
                if(pos < leftExt) { // risk underflow!
                    startPos = 0;
                } else {
                    startPos = pos - leftExt;
                }
                if(pos + suffixLen < pos) { // risk overflow!
                    endPos = targetLen;
                } else {
                    endPos = pos + rightExt;
                }
                // hitCache
                if (find_cache(hitCache, startPos)) {
                    // logger->trace("CACHE HIT: {}", pos);
                    continue;
                }
                // logger->trace("CACHE MISS: {}", pos);
                hitCache[startPos] = true;
                //
                TMyInfix targetInfix = infix(tal.targetSeq, startPos, endPos);
                Dna5String target;
                append(target, targetInfix);
                TCSString css_target = target;
                const char *c_target = toCString(css_target);
                // logger->debug("c_query: '{}' ({})", c_query, (int) strlen(c_query));
                // logger->debug("c_target: '{}' ({})", c_target, (int) strlen(c_target));
                //
                THitHandle hh = {toCString(queryId), toCString(css_pattern), toCString(tal.targetId),
                                 startPos, c_query, c_target, opt.editDist,
                                 tal.outFile
                                 };
                handle_hit(hh);
            } // ~ while (find(myFinder, pattern))
        } // ~ for(int fragStart = 0;
    } // ~ while (! atEnd(queryFileIn))
}


class Aligner {

public:
    TCLOpt &opt;


    Aligner(TCLOpt &clOpt) : opt(clOpt) {
    }


    int run() {
        // clOpt = opt;
        /*    
        length of fragment for exact search
        https://github.com/BilkentCompGen/mrfast/wiki/user-manual
        "For a given read length (l) and error threshold (e), the window size is floor(l/(e+1))"
        */
        SeqFileIn targetFileIn(opt.targetPath);

        std::ofstream outFile;
        outFile.open(opt.outputPath);

        // enumerate the target sequences
        while (! atEnd(targetFileIn)) {
            CharString targetId;
            Dna5String targetSeq;
            logger->info("[[BEGIN]] read target seq");
            readRecord(targetId, targetSeq, targetFileIn);
            size_t targetLen = length(targetSeq);
            logger->info("[[END]] read target seq: '{}', len: {}", toCString(targetId), targetLen);
            //
            // load the index from disk
            TMyFMIndex myIndex(targetSeq);
            CharString targetPath = opt.indexPath;
            append(targetPath, "/");
            append(targetPath, targetId);
            logger->info("[[BEGIN]] FMIndex load: '{}'", toCString(targetPath));
            open(myIndex, toCString(targetPath));
            logger->info("[[END]] FMIndex load: '{}'", toCString(targetPath));
            // ~ load

            TTargetAlign tAlign = {myIndex, targetId, targetSeq, outFile};
            //
            logger->warn("[[BEGIN]] TIMING: {}", toCString(targetId));
            // single-thread:
            // align_to_target(std::ref(opt), std::ref(tAlign), 0, 1);
            //
            // multi-thread:
            std::thread t1(align_to_target, std::ref(opt), std::ref(tAlign), 0, 3);
            std::thread t2(align_to_target, std::ref(opt), std::ref(tAlign), 1, 3);
            std::thread t3(align_to_target, std::ref(opt), std::ref(tAlign), 2, 3);
            t1.join();
            t2.join();
            t3.join();
            //
            logger->warn("[[END]] TIMING: {}", toCString(targetId));
        } // ~ while (! atEnd(targetFileIn))
        outFile.close();
    }
};



int main(int argc, char* const* argv)
{
    logger = start_the_logger();
    logger->info("[[BEGIN]] PROGRAM");
    TCLOpt opt;
    if(parse_args(argc, argv, opt) != 0) {
        return -1;
    }

    Aligner algn(opt);
    algn.run();

    logger->info("[[END]] PROGRAM");
    stop_the_logger();
    return 0;
}
