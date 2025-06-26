#include "stdio.h"
#include "stdlib.h"
#include <string.h>

typedef struct
{
    void *data;               // Pointer to the list data
    size_t element_size;      // Size of each element
    size_t length;            // Current number of elements
    size_t max_capacity;      // Allocated capacity
} List;

// Create a new list with default capacity
List *create_list(size_t ELEMENT_SIZE)
{
    List *list = malloc(sizeof(List));
    if (!list)
    {
        printf("%s\n", "List allocation failed");
        exit(EXIT_FAILURE);
    }
    list->data = malloc(ELEMENT_SIZE * 10);
    if (!list->data)
    {
        printf("%s\n", "List data allocation failed");
        exit(EXIT_FAILURE);
    }
    list->max_capacity = 10;
    list->length = 0;
    list->element_size = ELEMENT_SIZE;

    return list;
}

// Reallocate list to a new capacity
void realloc_list(List *list, const size_t new_capacity)
{
    void *new_values_ptr = realloc(list->data, (new_capacity * list->element_size));
    list->data = new_values_ptr;
    list->max_capacity = new_capacity;
}

// Add element to the end of the list
void list_push(List *list, void *value)
{
    if (list->length >= list->max_capacity)
    {
        realloc_list(list, list->max_capacity * 2);
    }

    memcpy(((char *)list->data + list->element_size * list->length), value, list->element_size);
    list->length++;
}

// Get element at index
void *list_get(List *list, int index)
{
    if (index < 0 || index >= list->length)
    {
        exit(EXIT_FAILURE);
    }

    return (void *)((char *)list->data + list->element_size * index);
}

// Set element at index
void list_set(List *list, void *value, int index)
{
    if (index < 0 || index >= list->length)
    {
        exit(EXIT_FAILURE);
    }

    void *target_addr = (char *)list->data + index * list->element_size;
    memcpy(target_addr, value, list->element_size);
}

// Remove element at index
void list_remove_at(List *list, int index)
{
    if (index < 0 || index >= list->length)
    {
        exit(EXIT_FAILURE);
    }

    void *dest_addr = (char *)list->data + index * list->element_size;
    void *src_addr = (char *)list->data + (index + 1) * list->element_size;
    size_t num_bytes_to_move = (list->length - index - 1) * list->element_size;
    memmove(dest_addr, src_addr, num_bytes_to_move);

    list->length--;
}

// Apply a callback function to each element
void list_foreach(List *list, void (*for_each_callback)(void *, int))
{
    for (int i = 0; i < list->length; i++)
    {
        for_each_callback(list_get(list, i), i);
    }
}

// Find all matching elements, return their indices
int *list_find_all(List *list, void *target, size_t *size, int (*find_callback)(void *, void *))
{
    int found_count = 0;

    for (int i = 0; i < list->length; i++)
    {
        void *curr = list_get(list, i);
        int found = find_callback(curr, target);
        if (found != 0)
        {
            found_count++;
        }
    }

    *size = found_count;

    int *values_found = malloc(sizeof(int) * found_count);
    if (!values_found)
    {
        return NULL;
    }

    int added = 0;
    for (int i = 0; i < list->length; i++)
    {
        void *curr = list_get(list, i);
        int found = find_callback(curr, target);
        if (found != 0)
        {
            values_found[added++] = i;
        }
    }

    return values_found;
}

// Insert value at specific index
void list_insert_at(List *list, int index, void *value)
{
    if (index < 0 || index > list->length)
    {
        exit(EXIT_FAILURE);
    }

    if (list->length == list->max_capacity)
    {
        realloc_list(list, list->max_capacity * 2);
    }

    char *base = (char *)list->data;
    memmove(
        base + (index + 1) * list->element_size,
        base + index * list->element_size,
        (list->length - index) * list->element_size);

    memcpy(base + index * list->element_size, value, list->element_size);
    list->length++;
}

// Free list memory
void list_free(List *list)
{
    if (list)
    {
        free(list->data);
        free(list);
    }
}

// Sort list using insertion sort
void list_sort(List *list, int (*compare_callback)(void *, void *))
{
    void *key = malloc(list->element_size);
    if (!key)
    {
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < list->length; i++)
    {
        void *current = list_get(list, i);
        memcpy(key, current, list->element_size);

        int j = i - 1;

        while (j >= 0)
        {
            void *elem_j = list_get(list, j);
            if (compare_callback(elem_j, key) > 0)
            {
                list_set(list, elem_j, j + 1);
                j--;
            }
            else
            {
                break;
            }
        }

        list_set(list, key, j + 1);
    }

    free(key);
}

// Binary search for a target value
int list_bin_search(List *list, void *target, int (*call_back_compare)(void *, void *))
{
    int left = 0;
    int right = list->length - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        void *mid_elem = list_get(list, mid);
        int cmp = call_back_compare(mid_elem, target);

        if (cmp == 0)
        {
            return mid;
        }
        else if (cmp < 0)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}

// Fill the list to capacity with a value
void list_fill(List *list, void *value)
{
    size_t initial_capacity = list->max_capacity;

    for (int i = 0; i < initial_capacity; i++)
    {
        list_push(list, value);
    }
}

// Clear the list (logical only)
void list_clear(List *list)
{
    list->length = 0;
}

// Clone a list (deep copy)
List *list_clone(List *original)
{
    List *clone = create_list(original->element_size);
    clone->data = malloc(original->element_size * original->max_capacity);
    if (!clone->data)
    {
        exit(EXIT_FAILURE);
    }
    clone->length = original->length;
    clone->max_capacity = original->max_capacity;

    memcpy(clone->data, original->data, original->element_size * original->length);

    return clone;
}

// Find index of first match using callback
int list_find_first(List *list, void *target, int (*find_callback)(void *, void *))
{
    for (int i = 0; i < list->length; i++)
    {
        void *curr = list_get(list, i);
        int found = find_callback(curr, target);
        if (found == 0)
        {
            return i;
        }
    }

    return -1;
}