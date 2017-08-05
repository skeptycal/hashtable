#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef unsigned long (*table_hash_func)(void *key);
typedef int (*table_compare_func)(void *key_a, void *key_b);

struct bucket
{
    char *key;
    void *value;
    struct bucket *next;
};
struct table
{
    int count;
    int capacity;
    table_hash_func hash;
    table_compare_func compare;
    struct bucket **buckets;
};

void table_init(struct table *table, int capacity, table_hash_func hash, table_compare_func compare);
void table_free(struct table *table);

void *table_find(struct table *table, char *key);
void table_insert(struct table *table, char *key, void *value);
void *table_remove(struct table *table, char *key);

#endif

#ifdef HASHTABLE_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

static struct bucket *
table_new_bucket(char *key, void *value)
{
    struct bucket *bucket = malloc(sizeof(struct bucket));
    bucket->key = key;
    bucket->value = value;
    bucket->next = NULL;
    return bucket;
}

static struct bucket **
table_get_bucket(struct table *table, char *key)
{
    struct bucket **bucket = table->buckets + (table->hash(key) % table->capacity);
    while(*bucket) {
        if(table->compare(key, (*bucket)->key)) {
            break;
        }
        bucket = &(*bucket)->next;
    }
    return bucket;
}

void table_init(struct table *table, int capacity, table_hash_func hash, table_compare_func compare)
{
    table->count = 0;
    table->capacity = capacity;
    table->hash = hash;
    table->compare = compare;
    table->buckets = malloc(sizeof(struct bucket *) * capacity);
    memset(table->buckets, 0, sizeof(struct bucket *) * capacity);
}

void table_free(struct table *table)
{
    for(int i = 0; i < table->capacity; ++i) {
        struct bucket *next, *bucket = table->buckets[i];
        while(bucket) {
            next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }
    free(table->buckets);
}

void *table_find(struct table *table, char *key)
{
    struct bucket *bucket = *table_get_bucket(table, key);
    return bucket ? bucket->value : NULL;
}

void table_insert(struct table *table, char *key, void *value)
{
    struct bucket **bucket = table_get_bucket(table, key);
    if(*bucket) {
        (*bucket)->value = value;
    } else {
        *bucket = table_new_bucket(key, value);
        ++table->count;
    }
}

void *table_remove(struct table *table, char *key)
{
    void *result = NULL;
    struct bucket *next, **bucket = table_get_bucket(table, key);
    if(*bucket) {
        result = (*bucket)->value;
        next = (*bucket)->next;
        free(*bucket);
        *bucket = next;
        --table->count;
    }
    return result;
}

#endif