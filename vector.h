#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef void* vector_elt_t;


typedef struct
{
    vector_elt_t * table;
    size_t size;
    size_t capacity;
} vector_t;


vector_t * create_vector(size_t init_capacity);

void vector_set_element_at(vector_t * v, size_t index, vector_elt_t e);
vector_elt_t vector_get_element_at(vector_t * v, size_t index);
void vector_del_element_at(vector_t * v, size_t index);
void vector_add_element(vector_t * v, vector_elt_t e);

size_t vector_size(vector_t * v);
size_t vector_capacity(vector_t * v);

void vector_clear(vector_t * v, int free_elt);

void free_vector(vector_t * v, int free_elt);

#endif
