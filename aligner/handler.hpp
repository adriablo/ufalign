#pragma once

#include <iostream>
#include <fstream>


struct THitHandle {
    const char *queryId;
    const char *pattern;
    const char *targetId;
    unsigned int pos;
    const char *query;
    const char *target;
    int eDist;
    std::ofstream &outFile;
};


bool handle_hit(THitHandle &hh);
