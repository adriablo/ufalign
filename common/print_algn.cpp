
#include <cstdio>
#include <algorithm>
#include <iostream>

#include "edlib.h"


int formatOneAlignment(char *buffer, int bufSize, const char* delimiter,
                      const char* query, const char* target,
                      const unsigned char* alignment, const int alignmentLength,
                      const int position, const EdlibAlignMode modeCode
                      );


int printAlignment(char *buffer, int bufSize,
                   const char* query, const char* target,
                   const EdlibAlignResult &ear, const EdlibAlignMode modeCode) {
    // Note: there may be multiple alignment paths
    // we only print the first one: endLocations[0]
    return formatOneAlignment(buffer, bufSize, "\n",
                             query, target,
                             ear.alignment, ear.alignmentLength,
                             ear.endLocations[0], modeCode
                             );
}


//
// See:
// https://github.com/Martinsos/edlib/blob/master/apps/aligner/aligner.cpp
//
// TODO: replace snprintf() with std::ostringstream stringStream
//
int formatOneAlignment(char *buffer, int bufSize, const char* delimiter,
                      const char* query, const char* target,
                      const unsigned char* alignment, const int alignmentLength,
                      const int position, const EdlibAlignMode modeCode) {
    if(bufSize <= 0) return -1;
    char* bufPos = buffer;
    const char* bufEnd = bufPos + bufSize;
    int ncw = 0;

    int tIdx = -1;
    int qIdx = -1;
    if (modeCode == EDLIB_MODE_HW) {
        tIdx = position;
        for (int i = 0; i < alignmentLength; i++) {
            if (alignment[i] != EDLIB_EDOP_INSERT)
                tIdx--;
        }
    }
    for (int start = 0; start < alignmentLength; start += 50) {
        // query
        // printf("Q: ");
        int startQIdx = qIdx;
        for (int j = start; j < start + 50 && j < alignmentLength; j++) {
            if (alignment[j] == EDLIB_EDOP_DELETE) {
                ncw = snprintf(bufPos, bufEnd - bufPos, "-");
                if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
                bufPos += ncw;
            }
            else {
                ncw = snprintf(bufPos, bufEnd - bufPos, "%c", query[++qIdx]);
                if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
                bufPos += ncw;
            }
            if (j == start)
                startQIdx = qIdx;
        }
        // print alignment positions
        // ncw = snprintf(bufPos, bufEnd - bufPos, " (%d - %d)", std::max(startQIdx, 0), qIdx);
        // if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
        // bufPos += ncw;

        // print delimiter
        ncw = snprintf(bufPos, bufEnd - bufPos, "%s", delimiter);
        if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
        bufPos += ncw;

        // match / mismatch
        // printf("   ");
        for (int j = start; j < start + 50 && j < alignmentLength; j++) {
            ncw = snprintf(bufPos, bufEnd - bufPos, alignment[j] == EDLIB_EDOP_MATCH ? "|" : " ");
            if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
            bufPos += ncw;
        }

        // print delimiter
        ncw = snprintf(bufPos, bufEnd - bufPos, "%s", delimiter);
        if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
        bufPos += ncw;

        // target
        // printf("T: ");
        int startTIdx;
        for (int j = start; j < start + 50 && j < alignmentLength; j++) {
            if (alignment[j] == EDLIB_EDOP_INSERT) {
                ncw = snprintf(bufPos, bufEnd - bufPos, "-");
                if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
                bufPos += ncw;
            }
            else {
                ncw = snprintf(bufPos, bufEnd - bufPos, "%c", target[++tIdx]);
                if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
                bufPos += ncw;
            }
            if (j == start)
                startTIdx = tIdx;
        }
        // print alignment positions
        // ncw = snprintf(bufPos, bufEnd - bufPos, " (%d - %d)", std::max(startTIdx, 0), tIdx);
        // if(ncw < 0 || ncw >= bufEnd - bufPos) return -1;
        // bufPos += ncw;
    }
    return 0;
}
