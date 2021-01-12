#include <stdlib.h>
#include <string.h>
#include "Map.h"

int hash_int(int key) {
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

int hash(int x, int y, int z) {
    x = hash_int(x);
    y = hash_int(y);
    z = hash_int(z);
    return x ^ y ^ z;
}

void map_alloc(Map *map, int dx, int dy, int dz, int mask) {
    map->dx = dx;
    map->dy = dy;
    map->dz = dz;
    map->mask = mask;
    map->size = 0;
    map->data = (MapEntry *)calloc(map->mask + 1, sizeof(MapEntry));
}

void map_free(Map *map) {
    free(map->data);
}

void map_copy(Map *dst, Map *src) {
    dst->dx = src->dx;
    dst->dy = src->dy;
    dst->dz = src->dz;
    dst->mask = src->mask;
    dst->size = src->size;
    dst->data = (MapEntry *)calloc(dst->mask + 1, sizeof(MapEntry));
    memcpy(dst->data, src->data, (dst->mask + 1) * sizeof(MapEntry));
}


/** 
 * @brief           将某个chunk中相对位置(x,y,z)的块标记为w类型（修改地图）
 * @param map       某个chunk的block_map
 * @param x         x坐标
 * @param y         y坐标
 * @param z         z坐标
 * @param w         推测代表方块的类型
 *
 * @return 
 */
int map_set(Map *map, int x, int y, int z, int w) {
    unsigned int index = hash(x, y, z) & map->mask;     // 一个小于mask的哈希值，是本次要生成的块在MapEntry中的索引
                                                        // 所以块在MapEntry中的位置是由其坐标决定的
    x -= map->dx;
    y -= map->dy;
    z -= map->dz;   // 这样之后x，y，z得到的应该是在chunk内的相对坐标
    MapEntry *entry = map->data + index;
    int overwrite = 0;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            overwrite = 1;      // 如果(x,y,z)位置上已经有块了就标记覆盖
            break;
        }
        index = (index + 1) & map->mask;        // 开放定址法处理冲突。。。。。
        entry = map->data + index;
    }
    if (overwrite) {
        if (entry->e.w != w) {      //更新块的类型
            entry->e.w = w;
            return 1;
        }
    }
    else if (w) {
        entry->e.x = x;
        entry->e.y = y;
        entry->e.z = z;
        entry->e.w = w;
        map->size++;
        if (map->size * 2 > map->mask) {
            map_grow(map);      //扩容
        }
        return 1;
    }
    return 0;
}

// 返回的是块的类型
int map_get(Map *map, int x, int y, int z) {
    unsigned int index = hash(x, y, z) & map->mask;
    x -= map->dx;
    y -= map->dy;
    z -= map->dz;
    if (x < 0 || x > 255) return 0;
    if (y < 0 || y > 255) return 0;
    if (z < 0 || z > 255) return 0;
    MapEntry *entry = map->data + index;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->e.x == x && entry->e.y == y && entry->e.z == z) {
            return entry->e.w;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
    }
    return 0;
}

void map_grow(Map *map) {
    Map new_map;
    new_map.dx = map->dx;
    new_map.dy = map->dy;
    new_map.dz = map->dz;
    new_map.mask = (map->mask << 1) | 1;
    new_map.size = 0;
    new_map.data = (MapEntry *)calloc(new_map.mask + 1, sizeof(MapEntry));
    MAP_FOR_EACH(map, ex, ey, ez, ew) {
        map_set(&new_map, ex, ey, ez, ew);
    } END_MAP_FOR_EACH;
    free(map->data);
    map->mask = new_map.mask;
    map->size = new_map.size;
    map->data = new_map.data;
}
