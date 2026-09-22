#ifndef MAP_H
#define MAP_H

#define HASH_SIZE 100

typedef struct Node Node;

extern Node* map[HASH_SIZE];


void map_put(const char* key,int val);


int map_get(const char* key);
#endif
