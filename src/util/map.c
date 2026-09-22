#include "map.h"
#include <stdlib.h>
#include <string.h>

#define KEY_MAX_LEN 64

typedef struct Node{
    char key[KEY_MAX_LEN];
    int val;
    struct Node *next;
}Node;

Node* map[HASH_SIZE];

static int hash(const char* s){
    unsigned int h = 0;
    
    while(*s != '\0'){
        h = h*31 + *s;
        s++;
    }
    return h % HASH_SIZE;
}

void map_put(const char* key,int val){
    int idx = hash(key);
    Node *n = malloc(sizeof(Node));

    strncpy(n->key, key, KEY_MAX_LEN - 1);
    n->key[KEY_MAX_LEN - 1] = '\0';
    n->val = val;
    n->next = map[idx];
    map[idx] = n;
}

int map_get(const char* key){
    int idx = hash(key);
    Node *p = map[idx];

    while(p){
        if(strcmp(p->key, key) == 0){
            return p->val;
        }
        p = p->next;
    }
    return 0;
}
