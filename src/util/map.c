#include "map.h"

#include <stdlib.h>
#include <string.h>

#define KEY_MAX_LEN 64

typedef struct Node {
    char key[KEY_MAX_LEN];
    int val;
    struct Node *next;
} Node;

/* 桶数组只在本编译单元可见：加 static 后不再污染全局符号，
 * 别的文件即使碰巧也有叫 map 的变量也不会链接冲突。 */
static Node *map[HASH_SIZE];

/* 经典 BKDR 哈希：h = h * 31 + c。
 * 注意 *s 必须提升为 unsigned char：char 可能是有符号的，
 * 遇到 >127 的字节（如 UTF-8 中文）会把符号位带进来，
 * 在不同平台/编译器下行为不一致。 */
static size_t hash(const char *s) {
    unsigned int h = 0;
    while (*s != '\0') {
        h = h * 31 + (unsigned char)*s;
        s++;
    }
    return (size_t)(h % HASH_SIZE);
}

void map_put(const char *key, int val) {
    if (key == NULL) return; /* 防御：空指针直接忽略，不让调用方崩溃 */

    size_t idx = hash(key);

    /* 已存在同 key 时更新值，而不是再头插一个新节点。
     * 原来的写法：同一个 key put 两次，链表里就有两个节点，
     * map_get 永远返回旧值，链表还会越变越长（内存也白占）。 */
    for (Node *p = map[idx]; p != NULL; p = p->next) {
        if (strcmp(p->key, key) == 0) {
            p->val = val;
            return;
        }
    }

    Node *n = malloc(sizeof(Node));
    if (n == NULL) return; /* 防御：分配失败不能直接解引用 NULL */

    /* 手动拷贝 + 补 '\0'，比 strncpy 更可控：
     * strncpy 在源串较短时会用 '\0' 把剩余空间全部填满，属于浪费；
     * 且容易忘补结尾。这里用 memcpy + 截断长度，行为一目了然。 */
    size_t len = strlen(key);
    if (len >= KEY_MAX_LEN) len = KEY_MAX_LEN - 1; /* 超长 key 截断 */
    memcpy(n->key, key, len);
    n->key[len] = '\0';

    n->val = val;
    n->next = map[idx]; /* 头插法 */
    map[idx] = n;
}

int map_get(const char *key) {
    if (key == NULL) return 0;

    size_t idx = hash(key);
    for (Node *p = map[idx]; p != NULL; p = p->next) {
        if (strcmp(p->key, key) == 0) {
            return p->val;
        }
    }
    return 0; /* 注意：0 同时表示"不存在"和"值就是 0"，调用方按场景自行区分 */
}

/* 清空整张表并释放所有节点。
 * cli_run 每次运行前调用一次，避免上一次运行的 flag 残留到下一次。 */
void map_clear(void) {
    for (size_t i = 0; i < HASH_SIZE; i++) {
        Node *p = map[i];
        while (p != NULL) {
            Node *next = p->next;
            free(p);
            p = next;
        }
        map[i] = NULL;
    }
}
