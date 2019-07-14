// Copyright 2019 Justin Hu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is part of the T Language Compiler.

// Implementation of vector of pointers, in java generic style

#include "util/container/vector.h"

#include "util/functional.h"

#include <stdlib.h>
#include <string.h>

Vector *vectorCreate(void) {
  Vector *vector = malloc(sizeof(Vector));
  vectorInit(vector);
  return vector;
}
void vectorInit(Vector *vector) {
  vector->size = 0;
  vector->capacity = 1;
  vector->elements = malloc(sizeof(void *));
}
Vector *vectorCopy(Vector *v, void *(*elmCopy)(void *)) {
  Vector *vector = malloc(sizeof(Vector));
  vector->size = v->size;
  vector->capacity = v->capacity;
  vector->elements = malloc(vector->capacity * sizeof(void *));
  for (size_t idx = 0; idx < vector->size; idx++) {
    vector->elements[idx] = elmCopy(v->elements[idx]);
  }
  return vector;
}
void vectorInsert(Vector *vector, void *element) {
  if (vector->size == vector->capacity) {
    vector->capacity *= 2;
    vector->elements =
        realloc(vector->elements, vector->capacity * sizeof(void *));
  }
  vector->elements[vector->size++] = element;
}
void vectorUninit(Vector *vector, void (*dtor)(void *)) {
  for (size_t idx = 0; idx < vector->size; idx++) dtor(vector->elements[idx]);
  free(vector->elements);
}
void vectorDestroy(Vector *vector, void (*dtor)(void *)) {
  vectorUninit(vector, dtor);
  free(vector);
}

StringVector *stringVectorCreate(void) { return vectorCreate(); }
void stringVectorInit(StringVector *v) { vectorInit(v); }
void stringVectorInsert(StringVector *v, char *s) { vectorInsert(v, s); }
void stringVectorUninit(StringVector *v, bool freeStrings) {
  vectorUninit(v, freeStrings ? free : nullDtor);
}
void stringVectorDestroy(StringVector *v, bool freeStrings) {
  vectorDestroy(v, freeStrings ? free : nullDtor);
}

// ctor
BoolVector *boolVectorCreate(void) {
  BoolVector *vector = malloc(sizeof(BoolVector));
  boolVectorInit(vector);
  return vector;
}
// in place ctor
void boolVectorInit(BoolVector *vector) {
  vector->size = 0;
  vector->capacity = 1;
  vector->elements = malloc(sizeof(bool));
}
// insert
void boolVectorInsert(BoolVector *vector, bool b) {
  if (vector->size == vector->capacity) {
    vector->capacity *= 2;
    vector->elements =
        realloc(vector->elements, vector->capacity * sizeof(bool));
  }
  vector->elements[vector->size++] = b;
}
// in place dtor
void boolVectorUninit(BoolVector *vector) { free(vector->elements); }
// dtor
void boolVectorDestroy(BoolVector *vector) {
  boolVectorUninit(vector);
  free(vector);
  return;
}