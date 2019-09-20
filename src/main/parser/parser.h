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

// The parser

#ifndef TLC_PARSER_PARSER_H_
#define TLC_PARSER_PARSER_H_

#include "util/container/hashMap.h"
#include "util/errorReport.h"
#include "util/fileList.h"
#include "util/options.h"

struct Node;

// specialization of a generic
// associates between a module and the AST
typedef HashMap ModuleAstMap;
// ctor
ModuleAstMap *moduleAstMapCreate(void);
// in-place ctor
void moduleAstMapInit(ModuleAstMap *);
// get
Node *moduleAstMapGet(ModuleAstMap const *, char const *key);
// put
int moduleAstMapPut(ModuleAstMap *, char const *key, struct Node *value);
// in-place dtor
void moduleAstMapUninit(ModuleAstMap *);
// dtor
void moduleAstMapDestroy(ModuleAstMap *);

// pod struct holding two ModuleAstMaps
typedef struct ModuleAstMapPair {
  ModuleAstMap decls;
  ModuleAstMap codes;
} ModuleAstMapPair;
// ctor
ModuleAstMapPair *moduleAstMapPairCreate(void);
// in-place ctor
void moduleAstMapPairInit(ModuleAstMapPair *);
// in-place dtor
void moduleAstMapPairUninit(ModuleAstMapPair *);
// dtor
void moduleAstMapPairDestroy(ModuleAstMapPair *);

// parses the files, producing SOME_TYPE
void parse(ModuleAstMapPair *, Report *, Options const *, FileList const *);

#endif  // TLC_PARSER_PARSER_H_