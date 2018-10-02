
#include "handler.hpp"

#include <cstdlib>
#include <string.h>
#include <iostream>

// https://github.com/Martinsos/edlib
#include "edlib.h"

#include "logger.hpp"
#include "print_algn.hpp"


std::mutex gSAMOutMutex;


//
// See:
// https://github.com/Martinsos/edlib/blob/master/apps/aligner/aligner.cpp
//
int get_alignment_pos(EdlibAlignResult &result, EdlibAlignConfig &config) {
    int endLocation = result.endLocations[0];
    int pos = -1;
    if (config.mode == EDLIB_MODE_HW) {
        pos = endLocation;
        for (int i = 0; i < result.alignmentLength; i++) {
            if (result.alignment[i] != EDLIB_EDOP_INSERT)
                pos--;
        }
    }
    if(result.alignment[0] != EDLIB_EDOP_INSERT) {
        pos++;
    }
    return pos;
}


bool handle_hit(THitHandle &hh) {
    // TODO: handle under- and overflows!!
    // fast align:
    // EdlibAlignConfig edlibConfig = edlibNewAlignConfig(edit_dist, EDLIB_MODE_HW, EDLIB_TASK_DISTANCE, NULL, 0);

    // slow - debug
    EdlibAlignConfig edlibConfig = edlibNewAlignConfig(hh.eDist, EDLIB_MODE_HW, EDLIB_TASK_PATH, NULL, 0);

    EdlibAlignResult result = edlibAlign(hh.query, (int) strlen(hh.query),
                                         hh.target, (int) strlen(hh.target),
                                         edlibConfig
                                         );
    //
    // TODO: for speed, do alignment in 2 passes:
    // pass 1: find alignment positions only, do not decode alignment
    // (write intermediate results to file)
    // pass 2: decode alignments from pass 1 only if/where needed (filtering)
    //
    if(result.editDistance >= 0) {
        int endLocation = result.endLocations[0];
        // compute alignment position on target sequence
        int tPos = get_alignment_pos(result, edlibConfig);
        tPos += hh.pos;
        //                    
        logger->info("EDIST HIT\tqueryId:{}\ttargetId:{}\tpattern:{}\tquery:'{}'\ttarget:'{}'\tpos:{}\tendLocation:{}\ttPos:{}\tedist:{}", hh.queryId, hh.targetId, hh.pattern, hh.query, hh.target, hh.pos, endLocation, tPos, result.editDistance);
        //
        const size_t algnBufSz = 4096;
        char algnBuf[algnBufSz];
        int pr = printAlignment(algnBuf, algnBufSz, hh.query, hh.target, result, edlibConfig.mode);
        assert(pr == 0);
        logger->debug("alignment:\n{}", algnBuf);
        // print to output (SAM)
        {
            std::lock_guard<std::mutex> locked(gSAMOutMutex);
            hh.outFile << hh.queryId << "\t" << 0 << "\t"  << hh.targetId << "\t"  << tPos + 1 << "\t"  << result.editDistance << std::endl;
        }
    } else {
        logger->trace("EDIST MISS\tpos:{}\tedit_distance: {}", hh.pos, result.editDistance);
    }
    edlibFreeAlignResult(result);
    return (result.editDistance >= 0);
}
