/*
DScheduler.c


*/
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"


#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dscheduler.h"

/* How much tracks we have on the disk */
#define NUM_TRACKS 1024

/* Comparator for qsort(), for int type */
static int cmp_int(const void * p1, const void * p2){
  const int x = *(int*) p1;
  const int y = *(int*) p2;
  return (x - y);
}

/*
Any required standard libraries and your header files here
*/

struct schedulerInput loadRequest(){
    struct schedulerInput results;
    int numRequests;
    char line_buffer[MAX_LINE_LENGTH];
    char direction;
    char *token;

    //Process simulation input line by line
    fgets(line_buffer, MAX_LINE_LENGTH, stdin);

    token = strtok(line_buffer, " ");
    sscanf(token, "%d",&numRequests);

    token = strtok(NULL, " ");
    sscanf(token, "%d",&results.startTrack);

    token = strtok(NULL, " ");
    sscanf(token, "%c",&direction);
    results.direction = direction == 'u' ? 1 : -1;

    results.requests.elements = numRequests;
    results.requests.data = (int *)malloc(sizeof(int)*numRequests);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }

    for (int i = 0; i < numRequests; i++){
        token = strtok(NULL, " ");
        sscanf(token, "%d", &results.requests.data[i]);
    }

    return results;
}

void printResults(struct schedulerResult results){
    for (int i = 0; i < results.requests.elements; i++){
        printf("%4d", results.requests.data[i]);
    }
    printf(" Total Head Movement %5d\n", results.totalHeadMovement);
}

struct schedulerResult processRequest(enum POLICIES policy, struct schedulerInput request){
    struct schedulerResult results;


    switch(policy){
        case FCFS:
            return process_FCFS_request(request);
        case SSTF:
            return process_SSTF_request(request);
        case SCAN:
            return process_SCAN_request(request);
        case C_SCAN:
            return process_C_SCAN_request(request);
    }
    return results;
}

/* Fill in the following functions */
struct schedulerResult process_FCFS_request(struct schedulerInput request){
    struct schedulerResult results;

    /* Allocate space for the result sequence */
    results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }

    /* Copy the request sequence to result sequence, calculating the head movement */
    int i, head_position = request.startTrack;
    for (i = 0; i < request.requests.elements; i++) {
        const int current_request = request.requests.data[i];

        // calculate absolute distance of head movement
        results.totalHeadMovement += abs(current_request - head_position);

        // accessed track is now new head_pos
        head_position = current_request;
        results.requests.data[i] = current_request;
    }
    results.requests.elements = request.requests.elements;

    return results;
}

/* Shortest Seek Time First policy */
struct schedulerResult process_SSTF_request(struct schedulerInput request){
    struct schedulerResult results;

    /* Allocate space for the result sequence */
    results.requests.elements = 0;
    results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }


    int i,j, head_position = request.startTrack;

    /* For each request */
    for (i = 0; i < request.requests.elements; i++) {

        //find the request with shortest distance to current head position
        int min_request = -1, min_distance = 0;
        for (j = 0; j < request.requests.elements; j++) {

          //skip visited tracks
          if(request.requests.data[j] < 0){
            continue;
          }

          const int distance = abs(request.requests.data[j] - head_position);
          //if distance is shorter, than currently found
          if((min_request == -1) || (distance <= min_distance)){
            //save it
            min_request = j;
            min_distance = distance;
          }
        }
        //the request with shortest distance becomes the current request
        const int current_request = request.requests.data[min_request];

        // calculate absolute distance of head movement
        results.totalHeadMovement += abs(current_request - head_position);

        // accessed track is now new head_pos
        head_position = current_request;
        results.requests.data[results.requests.elements++] = current_request;

        //remove the request from sequence
        request.requests.data[min_request] = -1;  //remove the request from list
    }

    return results;
}

struct schedulerResult process_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;

    //we need array for requests left and right of current head position
    struct IntArray left, right;  //left is down, right is up
    left.elements  = 0;
    right.elements = 0;

    //allocate space for the two arrays
    left.data  = (int *)malloc(sizeof(int)*request.requests.elements);
    right.data = (int *)malloc(sizeof(int)*request.requests.elements);
    results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);

    //if allocation failed
    if ( (left.data == NULL) || (right.data == NULL) || (results.requests.data == NULL)){
      fprintf( stderr, "Was unable to allocate space for requests data.\n");
      exit( BAD_MALLOC );
    }

    //Separate the request using their position, based on head
    int i;
    for (i = 0; i < request.requests.elements; i++) {
      //if its left of head
      if (request.requests.data[i] < request.startTrack)
          left.data[left.elements++] = request.requests.data[i];

      //if its right from head
      if (request.requests.data[i] > request.startTrack)
          right.data[right.elements++] = request.requests.data[i];
    }

    //sort left and right
    qsort(left.data, left.elements, sizeof(int), cmp_int);
    qsort(right.data, right.elements, sizeof(int), cmp_int);


    //process the left and right request sequences
    int j, head_position = request.startTrack;
    for(j = 0; j < 2; j++){
        if (request.direction == -1) {  //if head is going left (down)
          for (i = left.elements - 1; i >= 0; i--) {
              const int current_request = left.data[i];

              // calculate absolute distance of head movement
              results.totalHeadMovement += abs(current_request - head_position);
              results.requests.data[results.requests.elements++] = current_request;

              // accessed track is now new head_pos
              head_position = current_request;
          }
          request.direction = 1;  //change the direction of head to up

          if((j == 0) && (right.elements > 0)){ //if there is another pass and we have requests
            results.totalHeadMovement += abs(0 - head_position);
            head_position = 0; //now head is at first track
          }

        } else if (request.direction == 1) {  //if head is going up/right

          //process requests in the right sequence
          for (i = 0; i < right.elements; i++) {
              const int current_request = right.data[i];

              // calculate absolute distance of head movement
              results.totalHeadMovement += abs(current_request - head_position);
              results.requests.data[results.requests.elements++] = current_request;

              // accessed track is now new head_pos
              head_position = current_request;
          }
            request.direction = -1; //change head direction to down

            if((j == 0) && (left.elements > 0)){ //if there is another pass and we have requests
              results.totalHeadMovement += abs((NUM_TRACKS - 1) - head_position);
              head_position = NUM_TRACKS - 1;
            }
        }
    }

    free(left.data);
    free(right.data);

    return results;
}

struct schedulerResult process_C_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;

    //we need array for requests left and right of current head position
    struct IntArray left, right;
    left.elements  = 0;
    right.elements = 0;

    //allocate space for the two arrays
    left.data  = (int *)malloc(sizeof(int)*request.requests.elements);
    right.data = (int *)malloc(sizeof(int)*request.requests.elements);
    results.requests.data = (int *)malloc(sizeof(int)*request.requests.elements);

    //if allocation failed
    if ( (left.data == NULL) || (right.data == NULL) || (results.requests.data == NULL)){
      fprintf( stderr, "Was unable to allocate space for requests data.\n");
      exit( BAD_MALLOC );
    }

    int i;

    //we need to visit both ends of the disk
    left.data[left.elements++] = 0;
    right.data[right.elements++] = NUM_TRACKS - 1;

    //Separate the request using their position, based on head
    for (i = 0; i < request.requests.elements; i++) {

        if (request.requests.data[i] < request.startTrack)
            left.data[left.elements++] = request.requests.data[i];

        if (request.requests.data[i] > request.startTrack)
            right.data[right.elements++] = request.requests.data[i];
    }

    //sort left and right
    qsort(left.data, left.elements, sizeof(int), cmp_int);
    qsort(right.data, right.elements, sizeof(int), cmp_int);


    int head_position = request.startTrack;
    for (i = 0; i < right.elements; i++) {
        const int current_request = right.data[i];

        // calculate absolute distance of head movement
        results.totalHeadMovement += abs(current_request - head_position);

        // accessed track is now new head_pos
        head_position = current_request;

        if((current_request != 0) && (current_request != (NUM_TRACKS-1))){
          results.requests.data[results.requests.elements++] = current_request;
        }
    }

    //Now we jump to the beginning
    results.totalHeadMovement += NUM_TRACKS - 1;
    head_position = 0;

    for (i = 0; i < left.elements; i++) {
        const int current_request = left.data[i];

        // calculate absolute distance of head movement
        results.totalHeadMovement += abs(current_request - head_position);

        // accessed track is now new head_pos
        head_position = current_request;

        if((current_request != 0) && (current_request != (NUM_TRACKS - 1))){
          results.requests.data[results.requests.elements++] = current_request;
        }
    }

    free(left.data);
    free(right.data);

    return results;
}
