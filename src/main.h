//
//  main.h
//  Paging Simulator
//
//  Created by Greg on 11/24/14.
//  Last Modified 12/8/14
//  Copyright (c) 2014 Gregory Jordan. All rights reserved.
//

/* 
 
 Structure to hold our process
 
 */

struct Process{
    int action; // load unload
    int pid; // process id
    int psize; // process size
};

#define MEM_LOCATIONS 65536
#define MAX_PROC 10000
#define ADDR_SIZE 16.0
#define FRAG_THRESH 0.5
#define LOAD 1
#define UNLOAD 0

double simulate(struct Process[], int, int);
int importData(struct Process[], const char*);
void initProcessArray(struct Process[], int);
void load(struct Process, struct Process*, double, int, int);
void unload(int, struct Process*, int);
double fragmentation(struct Process*, int, int);
