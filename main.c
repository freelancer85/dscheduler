/*

main.c for use with dispatcher.c

Do not edit the contents of this file.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dscheduler.h"


int main(int argc, char **argv) {
    enum POLICIES policy;

    switch(argc) {
        case 2:
            if ( strcmp(argv[1], "FCFS") == 0 ){
                policy = FCFS;
            } else if ( strcmp(argv[1], "SSTF") == 0 ){
                policy = SSTF;
            } else if ( strcmp(argv[1], "SCAN") == 0 ){
                policy = SCAN;
            } else if ( strcmp(argv[1], "C-SCAN") == 0 ){
                policy = C_SCAN;
            } else {
               fprintf(stderr, "Invalid policy\n");
                exit( BAD_INPUT );
            }

            break;
        default:
            fprintf(stderr, "Incorrect # of inputs\n");
            exit( WRONG_NUM_INPUTS );
    }

    printResults( processRequest(policy, loadRequest() ) );
    return 0;
}
