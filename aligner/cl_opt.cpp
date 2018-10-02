
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <getopt.h>

#include "cl_opt.hpp"
#include "logger.hpp"


int parse_args(int argc, char* const* argv, TCLOpt &opt) {
    opt.editDist = 2;
    //
    extern char *optarg;
    int flag = 0;
    int c;
    while ((c = getopt (argc, argv, "q:t:i:o:d:")) != -1) {
        switch (c) {
        case 'q':
            opt.queryPath = optarg;
            flag++;
            break;
        case 't':
            opt.targetPath = optarg;
            flag++;
            break;
        case 'i':
            opt.indexPath = optarg;
            flag++;
            break;
        case 'o':
            opt.outputPath = optarg;
            flag++;
            break;
        case 'd':
            opt.editDist = atoi(optarg);
            flag++;
            break;
        default:
            // bad!
        flag = -1;
            break;
        }        
    }
    if(flag < 5) {
        std::cout << "Usage: q:t:i:o:d:" << std::endl;
        logger->error("Bad arguments");
        return -1;
    }
    return 0;
}
