/*
Do not edit the contents of this file
*/

#ifndef DSCHEDULER_H
    #define DSCHEDULER_H
    
    #define MAX_LINE_LENGTH 1000
    #define WRONG_NUM_INPUTS 1
    #define BAD_MALLOC 2
    #define BAD_INPUT 3

    enum POLICIES {
        FCFS = 0,
        SSTF = 1,
        SCAN = 2,
        C_SCAN = 3,
    };
    
    struct IntArray
    {
        int elements;
        int *data;
    };
    
    
    struct schedulerInput 
    {
    
        int startTrack;
        int direction;
        struct IntArray requests;
    };
    
    struct schedulerResult 
    {
        int totalHeadMovement;
        struct IntArray requests;
    };
    
    struct schedulerInput loadRequest();
    struct schedulerResult processRequest(enum POLICIES policy, struct schedulerInput request);
    void printResults(struct schedulerResult results);
    
    
    struct schedulerResult process_FCFS_request(struct schedulerInput request);
    struct schedulerResult process_SSTF_request(struct schedulerInput request);
    struct schedulerResult process_SCAN_request(struct schedulerInput request);
    struct schedulerResult process_C_SCAN_request(struct schedulerInput request);
#endif
