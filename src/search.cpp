#include <iostream>
#include <string>
#include <vector>
#include "lsh/LSHmain.h"
#include "hypercube/HCmain.h"
#include "ivf/ivf_main.h"
using namespace std;


int lsh_main(int argc, char** argv);
int hypercube_main(int argc, char** argv);
int ivfflat_main(int argc, char** argv);
int ivfpq_main(int argc, char** argv);

void print_usage(const char* prog) {
    cerr << "Usage: " << prog << " -lsh <lsh-args...> | -hypercube <hypercube-args...> "
         << "| -ivfflat <ivfflat-args...> | -ivfpq <ivfpq-args...>\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Find the flag which contains the algorithm that we will use
    bool found = false;
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "-lsh") {
            found = true;
            return lsh_main(argc, argv);
        } else if (a == "-hypercube") {
            found = true;
            return hypercube_main(argc, argv);
        } 
        else if (a == "-ivfflat") {
             found = true;
             return ivfflat_main(argc, argv);
         } else if (a == "-ivfpq") {
             found = true;
             return ivfpq_main(argc, argv);
         }
    }

    if (!found) {
        cerr << "No algorithm flag provided. Use one of: -lsh, -hypercube, -ivfflat, -ivfpq\n";
        print_usage(argv[0]);
        return 2;
    }

    return 0; // unreachable
}


