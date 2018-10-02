#pragma once


struct TCLOpt {
    const char *queryPath;
    const char *targetPath;
    const char *indexPath;
    const char *outputPath;
    int editDist;
};


int parse_args(int argc, char* const* argv, TCLOpt &opt);
