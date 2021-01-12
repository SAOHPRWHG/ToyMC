#pragma once

#define EMPTY_ENTRY(entry) ((entry)->value == 0)

#define MAP_FOR_EACH(map, ex, ey, ez, ew) \
    for (unsigned int i = 0; i <= map->mask; i++) { \
        MapEntry *entry = map->data + i; \
        if (EMPTY_ENTRY(entry)) { \
            continue; \
        } \
        int ex = entry->e.x + map->dx; \
        int ey = entry->e.y + map->dy; \
        int ez = entry->e.z + map->dz; \
        int ew = entry->e.w;

#define END_MAP_FOR_EACH }

typedef union {
    unsigned int value;
    struct {
        unsigned char x;
        unsigned char z;        // block在chunk内的相对坐标
        char w;                 // block类型
        unsigned char y;
    } e;
} MapEntry;

typedef struct {
    int dx;     
    int dy;
    int dz;     // 这三个是chunk的基坐标
    unsigned int mask;      // data的容量
    unsigned int size;      // data中已存储的block的数量
    MapEntry *data;     // chunk中每个block的信息
} Map;      // 存储一个chunk的地图

void map_alloc(Map *map, int dx, int dy, int dz, int mask);
void map_free(Map *map);
void map_copy(Map *dst, Map *src);
void map_grow(Map *map);
int map_set(Map *map, int x, int y, int z, int w);
int map_get(Map *map, int x, int y, int z);
