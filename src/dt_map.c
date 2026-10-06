/*
 * dt_map.c: Associative arrays for Unit 5, Section E.
 *
 * An array does not store its indices. This map stores its keys.
 * An array calculates a position with one subtraction.
 * The map calculates a hash and then compares keys in one bucket.
 *
 * Hashing turns the key into a bucket number. Compare all keys in that bucket
 * because two keys can select it. Use a linked list for each bucket. Start the
 * unsigned accumulator at 14695981039346656037ULL. For each unsigned byte,
 * exclusive-or the byte into it and multiply by 1099511628211ULL.
 *
 * A separate list stores insertion order for stable output. dt_map_key_at
 * reads this list. Updating a key preserves its position. Removing and
 * reinserting a key moves it to the end.
 */

#include "dt.h"

#include <stdlib.h>
#include <string.h>

struct dt_map {
    size_t count;
    size_t bucket_count;    //capacity
    struct dt_map_entry **buckets;
    struct dt_map_entry *order_head;
    struct dt_map_entry *order_tail;
};

struct dt_map_entry {
    char *key;
    dt_value value;
    struct dt_map_entry *next;          //next in bucket chain
    struct dt_map_entry *order_next;    //next in insertion order
    struct dt_map_entry *order_prev;    //prev in insertion order
};

/*
 * dt_map_new builds an empty map. It returns NULL after an allocation failure.
 */
dt_map *dt_map_new(void)
{
    /* TODO: Return an allocated empty map. Return NULL after an allocation failure.
       dt_map_new()  -> a map whose dt_map_len is 0
       cases/normal/map_basics.case */
    dt_map *m = malloc(sizeof(dt_map));
    if(m == NULL){
        return NULL;
    }

    m->bucket_count = 16; //aribitrary
    m->buckets = malloc(m->bucket_count * sizeof(struct dt_map_entry *));
    if(m->buckets == NULL){ //same concept from array: clean up
        free(m);
        return NULL;
    }

    for (size_t i = 0; i < m->bucket_count; i++) {
        m->buckets[i] = NULL;
    }

    m->count = 0;
    m->order_head = NULL;
    m->order_tail = NULL;

    return m;

}

/*
 * dt_map_free releases each entry, copied key, order array, and map.
 * It accepts NULL. The environment owns the values.
 */
void dt_map_free(dt_map *m)
{
    /* TODO: Release each entry, copied key, order array, and map.
       Preserve the values. The environment owns them.
       a map holding a string value  -> the nodes and keys go, the string stays
       dt_map_free(NULL)             -> returns, having done nothing
       cases/cleanup/map_churn.case */
    if (m == NULL) {
        return;
    }

    struct dt_map_entry *curr = m->order_head;
    while (curr != NULL) {
        struct dt_map_entry *next = curr->order_next;
        free(curr->key);
        free(curr);
        curr = next;
    }

    free(m->buckets);
    free(m);
}

/*
 * dt_map_len returns the number of keys in constant time.
 */
size_t dt_map_len(const dt_map *m)
{
    /* TODO: Return the current key count.
       Replacing a value does not change this count.
       after put alpha, beta, gamma:  dt_map_len(m) -> 3
       after put beta again:          dt_map_len(m) -> 3, still
       after del alpha:               dt_map_len(m) -> 2
       cases/normal/map_basics.case */
    return m->count;
}

/*
 * dt_map_put binds v to key.
 * An existing key keeps its insertion position. A new key becomes the last key.
 * Copy each new key because the caller owns the source buffer.
 * Return DT_ERR_CAPACITY after an allocation failure.
 */
dt_status dt_map_put(dt_map *m, const char *key, dt_value v)
{
    //use hash function to find which bucker key is from
    unsigned long long hash = 14695981039346656037ULL;
    for (const unsigned char *p = (const unsigned char *)key; *p != '\0'; p++) {
        hash ^= (unsigned long long)*p;
        hash *= 1099511628211ULL;
    }
size_t bucket_index = hash % m->bucket_count;
    //check if key exists
    struct dt_map_entry *curr = m->buckets[bucket_index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) { //use strcmp 
            curr->value = v;
            return DT_OK;
        }
        curr = curr->next;
    }

    //build new node
    struct dt_map_entry *newNode = malloc(sizeof(struct dt_map_entry));
    if (newNode == NULL) {
        return DT_ERR_CAPACITY;
    }

    //copy key text to new node
    newNode->key = malloc(strlen(key) + 1);
    if (newNode->key == NULL) {
        free(newNode);
        return DT_ERR_CAPACITY;
    }

    strcpy(newNode->key, key); //copies characters over
    newNode->value = v;

    //link into bucket chain
    newNode->next = m->buckets[bucket_index];
    m->buckets[bucket_index] = newNode;

    //link into insertion order
    newNode->order_prev = m->order_tail;
    newNode->order_next = NULL;

    //check if map was empty initially
    if (m->order_tail != NULL) {
        m->order_tail->order_next = newNode;
    } else {
        m->order_head = newNode;
    }
    m->order_tail = newNode;

    m->count++;
    return DT_OK;
}

/*
 * dt_map_get writes the value for key to *out.
 * It returns DT_ERR_KEY and does not change *out when the key is absent.
 * An absent key differs from a nil value.
 */
dt_status dt_map_get(const dt_map *m, const char *key, dt_value *out)
{
    /* TODO: Return DT_ERR_KEY when the key is absent.
       Preserve *out after this error. A nil value can be present.
       after put "beta" -> 22:
         dt_map_get(m, "beta", &out)   -> DT_OK, *out is the integer 22
         dt_map_get(m, "ghost", &out)  -> DT_ERR_KEY, *out untouched
       cases/normal/map_basics.case, cases/boundary/map_missing_key.case */
    unsigned long long hash = 14695981039346656037ULL;
    for (const unsigned char *p = (const unsigned char *)key; *p != '\0'; p++) {
        hash ^= (unsigned long long)*p;
        hash *= 1099511628211ULL;
    }
size_t bucket_index = hash % m->bucket_count;
    struct dt_map_entry *curr = m->buckets[bucket_index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            *out = curr->value;
            return DT_OK;
        }
        curr = curr->next;
    }

    return DT_ERR_KEY;
}

/*
 * dt_map_remove removes key from its bucket and insertion position.
 * It releases the copied key. It returns DT_ERR_KEY when the key is absent.
 */
dt_status dt_map_remove(dt_map *m, const char *key)
{
    /* TODO: Remove the entry from its bucket and insertion position.
       Release the copied key. Return DT_ERR_KEY when the key is absent.
       a map holding alpha, beta, gamma:
         dt_map_remove(m, "alpha")  -> DT_OK, order is now beta, gamma
         dt_map_remove(m, "ghost")  -> DT_ERR_KEY, nothing changes
       reinserting "alpha" appends it after "gamma"
       cases/normal/map_basics.case, cases/boundary/map_remove_missing_key.case */   
    unsigned long long hash = 14695981039346656037ULL;
    for (const unsigned char *p = (const unsigned char *)key; *p != '\0'; p++) {
        hash ^= (unsigned long long)*p;
        hash *= 1099511628211ULL;
    }
    size_t bucket_index = hash % m->bucket_count;
    struct dt_map_entry *prev = NULL;
    struct dt_map_entry *curr = m->buckets[bucket_index];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            //unlink found node
            if (prev == NULL) {
                m->buckets[bucket_index] = curr->next;  //if first node
            } else {
                prev->next = curr->next;
            }

            //unlink from the insertion-order list
            if (curr->order_prev != NULL) {
                curr->order_prev->order_next = curr->order_next;
            } else {
                m->order_head = curr->order_next;  //if first
            }
            if (curr->order_next != NULL) {
                curr->order_next->order_prev = curr->order_prev;
            } else {
                m->order_tail = curr->order_prev;  //if last
            }

            free(curr->key);
            free(curr);
            m->count--;
            return DT_OK;
        }
        prev = curr;
        curr = curr->next;
    }

    return DT_ERR_KEY;
}

/*
 * dt_map_key_at writes the key at insertion position index to *out.
 * It returns DT_ERR_RANGE and does not change *out for an invalid index.
 */
dt_status dt_map_key_at(const dt_map *m, size_t index, const char **out)
{
    /* TODO: Write the key at the specified insertion position to *out.
       Return DT_ERR_RANGE for an invalid position. Preserve *out after this error.
       The printer uses this order.
       a map holding alpha, beta, gamma:
         dt_map_key_at(m, 0, &out)  -> DT_OK, *out = "alpha"
         dt_map_key_at(m, 3, &out)  -> DT_ERR_RANGE, *out untouched
       cases/normal/map_basics.case */
    //bounds check
    if (index >= m->count) {
        return DT_ERR_RANGE;
    }

    //loop until specified index, then get key
    struct dt_map_entry *curr = m->order_head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->order_next;
    }

    *out = curr->key;
    return DT_OK;
}

