//
// main.c
// Paging Simulator
//
// Created by Greg on 11/24/14.
// Last Modified 12/8/14
// Copyright (c) 2014 Gregory Jordan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <math.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    
    // cehck that the correct number of command line arguments was entered
    if(argc != 2){
        printf("Invalid number of args. Usage: [filename]\n");
        return -1;
    }
    
    int i;
    int pageSize, numPages;
    double fragmentation;
    
    /*
     
     Allocate arrays for processes and page memory
     
     */
    
    struct Process plist[MAX_PROC];
    initProcessArray(plist, MAX_PROC);
    
    /*
     
     Read in file data
     
     */
    
    // ensure we have imported successfully
    if(!importData(plist, argv[1])){
        printf("Unable to import data from file.\n");
        return -1;
    }
    
    /*
     
     Run the simulation for the given page offset
     
     */
    
    for(i = 4; i<ADDR_SIZE; i++){
        
        // calculate the page size (# bits)
        pageSize = pow(2.0, ((double)ADDR_SIZE-(double)i));
        
        // calculate the number of pages
        numPages = pow(2.0, (double)i);
        
        // check if the simulation is optimal
        if((fragmentation = simulate(plist, pageSize, numPages)) != 0){
            printf("Optimum page size: %d\nLargest fragmentation: %lf\n", i, fragmentation);
            return 0;
        }
        
    }
    
    printf("No solution could be found for fragment theshold < 0.5\n");
    
    return 0;
}

void initProcessArray(struct Process p[], int num){
    /*
     
     Intialize all arrays with empty values
     
     */
    
    int i;
    
    for (i = 0; i<num; i++) {
        p[i].psize = -1;
        p[i].pid = -1;
        p[i].action = -1;
    }
    
}

int importData(struct Process plist[], const char* file){
    
    // unload is 6 characters
    char action[6];
    
    // process id
    int pid, i, act;
    
    // process size
    int psize;
    
    // open file for reading only
    FILE *fin = fopen(file, "r");
    
    // check if the file was opened
    if(fin == NULL){
        return 0;
    }
    
    // loop until we break at EOF
    for(i = 0;;i++){
        
        // check if we have exceeded the size of the array
        if(i == MAX_PROC){
            printf("Too many processes! Cannot load file.\n");
            return 0;
        }
        
        // find action we should take for loading our process
        if(fscanf(fin, " %s ", action) == EOF){
            
            // break in the case of EOF
            break;
        }
        
        // figure out what action we should take
        if(!strcmp(action, "Load")){
            
            // get process id throw out "PID"
            fscanf(fin, "%*s %d", &pid);
            
            // get process size
            fscanf(fin, "%*s %d", &psize);
            
            // change the action to load
            act = LOAD;
        // unload process
        }else{
            
            // get process id throw out "PID"
            fscanf(fin, "%*s %d", &pid);
            
            // set size to 0 since its unload
            psize = 0;
            
            // change the action to unload
            act = UNLOAD;
        }
        
        /*
         
         Set process values
         
         */
        
        plist[i].action = act;
        plist[i].pid = pid;
        plist[i].psize = psize;
        
    }
    
    return 1;
}

double simulate(struct Process plist[], int pageSize, int numPages){
    
    int i;
    
    // init array of pages
    struct Process *pages = (struct Process*)malloc(sizeof(struct Process)*numPages);
    initProcessArray(pages, numPages);
    
    // number of pages needed for the current process
    double procPages = 0;
    
    // keep track of the largest fragmentation
    double frag = 0, largestFrag = 0;
    
    // loop through the entire plist
    for(i = 0;i<MAX_PROC;i++){
        
        // look for an empty process and stop
        if(plist[i].pid == -1){
            break;
        }
        
        /*
         
         Load action
         
         */
        
        if(plist[i].action == LOAD){
            // find the number of pages needed for the current process
            procPages = ceil((double)plist[i].psize/(double)pageSize);
            
            // add the process to the pages array
            load(plist[i], pages, procPages, numPages, pageSize);
        }
        
        /*
         
         Unload action
         
         */
        if(plist[i].action == UNLOAD){
            
            // unload the process
            unload(plist[i].pid, pages, numPages);
        }
        
        /*
         
         Check that the framentation does not exceed o.5
         
         */
        
        if((frag = fragmentation(pages, numPages, pageSize)) > FRAG_THRESH){
            
            // return false because this page size is no good
            return 0;
        }else if(frag > largestFrag){
            
            // track the largest fragmentation
            largestFrag = frag;
        }
    }
    
    return largestFrag;
}

void unload(int pid, struct Process *pages, int numPages){
    
    /*
     
    Find the PID and remove the process from the pages array
     
     */
    
    int i;
    
    // loop through the entire list and remove anything with PID
    for(i = 0; i<numPages; i++){
        if(pages[i].pid == pid){
            pages[i].pid = -1;
            pages[i].psize = -1;
            pages[i].action = -1;
        }
    }
    
}

double fragmentation(struct Process *pages, int numPages, int pageSize){
    /*
     
     Calculate the fragmentation for the current pagesize
     
     */
    
    int i, unusedMem = 0;
    
    for (i = 0;i<numPages; i++) {
        
        // do not count empty pages as fragmentation
        if(pages[i].pid != -1){
        
            // add the amount of memory that is not being used per page
            unusedMem += pageSize-pages[i].psize;
            
        }
    }
    
    return ((double)unusedMem/MEM_LOCATIONS);
}

void load(struct Process proc, struct Process *pages, double procPages, int numPages, int pageSize){
    
    /*
     
     Finds (procPages) number of pages to put the process
     
     */
    
    int i;
    
    // the size of the last page to be entered since the previous ones will be the page size
    int lastPage = proc.psize-((procPages-1)*pageSize);
    
    // loop through and find empty pages
    for (i = 0; procPages != 0 ; i++) {
        
        // check if we have exceeded the number of pages
        if(i == numPages){
            return;
        }
        
        // check if the current page is empty
        if(pages[i].pid == -1){
            
            // put the process in that page
            pages[i].pid = proc.pid;
            
            // check if we are inserting the last page
            if(procPages){
                pages[i].psize = lastPage;
            }else{
                pages[i].psize = proc.psize;
            }
            pages[i].action = proc.action;
            
            // decrement the number of pages that need to be inserted
            procPages--;
        }
    }
    
}
