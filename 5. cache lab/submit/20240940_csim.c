#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// define struct
typedef struct{
    int valid;
    unsigned long long tag;
    int lru;
} line_type;

typedef struct{
    line_type* lines;
} set_type;

typedef struct{
    set_type* sets;

    int s; // number of set index
    int E; // number of lines
    int b; // number of block bit
} cache_type;

// global variable
int global_lru = 0;
int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;

// function prototype
void access_cache(cache_type* cache, unsigned long long int address);


int main(int argc, char *argv[])
{
    // variables
    cache_type cache = {0};
    int S = 0; // number of sets
    // int B = 0; // number of blocks, not used
    char* trace_file = NULL;

    // getopt
    int opt;
    int verbose = 0;
    while ((opt =getopt(argc, argv, "hvs:E:b:t:"))!=-1){
        switch(opt){
            case 'h':
                printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                cache.s = atoi(optarg);
                S = 1 << cache.s; // s^2
                break;
            case 'E':
                cache.E = atoi(optarg);
                break;
            case 'b':
                cache.b = atoi(optarg);
                // B = 1 << cache.b; // b^2
                break;
            case 't':
                trace_file = optarg;
                break;
        }
    }

    // initialize cache
    cache.sets = (set_type*) malloc(sizeof(set_type) * S);
    // line
    for (int i = 0; i < S; i++){
        cache.sets[i].lines = (line_type*)malloc(sizeof(line_type)*cache.E);
        for (int j = 0; j < cache.E; j++){
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru = 0;
        }
    }

    // read trace file
    FILE* fp = fopen(trace_file, "r");
    char type;
    unsigned long long int address;
    int size;

    while (fscanf(fp, " %c %llx,%d", &type, &address, &size)>0){
        if (verbose) printf(" %c %llx,%d\n", type, address, size);

        switch(type){
            case 'I':
                break;
            case 'L':
                access_cache(&cache, address);
                break;
            case 'S':
                access_cache(&cache, address);
                break;
            case 'M':
                access_cache(&cache, address); // load
                access_cache(&cache, address); // store
                break;
            default:
                break;
        }
    }
    fclose(fp);
    
    // free memory
    for (int i = 0; i < S; i++){
        free(cache.sets[i].lines);
    }
    free(cache.sets);

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

void access_cache(cache_type* cache, unsigned long long int address){
    global_lru++;
    int s = cache->s;
    int b = cache->b;

    // set index
    unsigned long long int mask = (1 << s) - 1;
    unsigned long long int set_index = (address >> b) & mask;
    // tag
    unsigned long long int tag = address >> (s + b);
    // setting cache set
    set_type* set = &cache->sets[set_index];
    int E = cache->E;

    // test hit
    for (int i = 0; i < E; i++){
        line_type *line = &set->lines[i];
        if(line->valid && tag==line->tag){
            hit_count++;
            line->lru = global_lru;
            return;
        }
    }
    
    // miss
    miss_count++;
    for(int i = 0; i < E; i++){
        line_type *line = &set->lines[i];
        if (!line->valid){
            line->valid = 1;
            line->tag = tag;
            line->lru = global_lru;
            return;
        }
    }

    // eviction
    eviction_count++;
    int min_lru = global_lru;
    int position = 0;

    for(int i = 0; i < E; i++){
        line_type* line = &set->lines[i];
        if(line->lru < min_lru){
            min_lru = line->lru;
            position = i;
        }
    }
    set->lines[position].tag = tag;
    set->lines[position].lru = global_lru;
}