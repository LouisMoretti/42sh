#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *h;
    if ((h = malloc(sizeof(struct hash_map))) == NULL)
        return NULL;

    if ((h->data = calloc(size, sizeof(struct pair_list *))) == NULL)
    {
        free(h);
        return NULL;
    }

    h->size = size;

    return h;
}

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    if (hash_map == NULL || hash_map->size == 0 || key == NULL)
        return false;

    size_t place = hash(key) % hash_map->size;
    struct pair_list *p = hash_map->data[place];
    while (p != NULL)
    {
        if (strcmp(p->key, key) == 0)
        {
            p->value = value;
            if (updated != NULL)
                *updated = true;
            return true;
        }
        p = p->next;
    }

    struct pair_list *v;
    if ((v = malloc(sizeof(struct pair_list))) == NULL)
        return false;

    if (updated != NULL)
        *updated = false;

    v->next = hash_map->data[place];
    v->key = key;
    v->value = value;

    hash_map->data[place] = v;

    return true;
}

static void _free_ll(struct pair_list *ll)
{
    if (ll == NULL)
        return;

    _free_ll(ll->next);
    free(ll);
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;

    for (size_t i = 0; i < hash_map->size; i++)
        _free_ll(hash_map->data[i]);

    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;

    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *v = hash_map->data[i];
        if (v == NULL)
            continue;

        printf("%s: %s", v->key, v->value);
        v = v->next;

        while (v != NULL)
        {
            printf(", %s: %s", v->key, v->value);
            v = v->next;
        }

        printf("\n");
    }
}

char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0 || key == NULL)
        return NULL;

    size_t place = hash(key) % hash_map->size;
    struct pair_list *p = hash_map->data[place];
    while (p != NULL)
    {
        if (strcmp(p->key, key) == 0)
            return p->value;
        p = p->next;
    }

    return NULL;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0 || key == NULL)
        return false;

    size_t place = hash(key) % hash_map->size;
    struct pair_list *p = hash_map->data[place];

    if (p == NULL)
        return false;

    if (strcmp(p->key, key) == 0)
    {
        hash_map->data[place] = p->next;
        free(p);
        return true;
    }

    while (p->next != NULL)
    {
        if (strcmp(p->next->key, key) == 0)
        {
            struct pair_list *t = p->next;
            p->next = t->next;
            free(t);
            return true;
        }

        p = p->next;
    }

    return false;
}
