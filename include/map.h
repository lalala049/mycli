#ifndef MAP_H
#define MAP_H

#define HASH_SIZE 100

/* key -> int 的简易哈希表（链地址法）。
 * 实现细节全部在 map.c 内部，头文件只暴露三个函数，
 * 不再声明 extern 数组 —— 那会污染全局符号，也和任何同名变量冲突。 */

void map_put(const char *key, int val);

int map_get(const char *key);

void map_clear(void);

#endif
