// Copyright 2019 Justin Hu
//
// This file is part of the T Language Compiler.

// implemetation for FileList

#include "util/fileList.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ctor
FileList *sortFiles(Report *report, int argc, char **argv) {
  FileList *list = malloc(sizeof(FileList));

  list->numDecl = 0;
  list->decls = NULL;
  list->numCode = 0;
  list->codes = NULL;

  for (size_t idx = 1; idx < (size_t)argc; idx++) {
    // skip - this is an option
    if (argv[idx][0] == '-') continue;

    size_t length = strlen(argv[idx]);
    if (length > 3 && argv[idx][length - 3] == '.' &&
        argv[idx][length - 2] == 't' && argv[idx][length - 1] == 'c') {
      // this is a code file
      bool duplicated = false;
      for (size_t cidx = 0; cidx < list->numCode; cidx++) {
        if (strcmp(list->codes[cidx], argv[idx]) == 0) {
          duplicated = true;
          break;
        }
      }
      if (duplicated) {
        size_t bufferSize = 18 + length;
        char *buffer = malloc(bufferSize);
        snprintf(buffer, bufferSize, "%s: duplicated file", argv[idx]);
        reportError(report, buffer);
      } else {
        list->codes =
            realloc(list->codes, ++list->numCode * sizeof(char const *));
        list->codes[list->numCode - 1] = argv[idx];
      }
    } else if (length > 3 && argv[idx][length - 3] == '.' &&
               argv[idx][length - 2] == 't' && argv[idx][length - 1] == 'd') {
      // this is a decl file
      bool duplicated = false;
      for (size_t cidx = 0; cidx < list->numDecl; cidx++) {
        if (strcmp(list->decls[cidx], argv[idx]) == 0) {
          duplicated = true;
          break;
        }
      }
      if (duplicated) {
        size_t bufferSize = 18 + length;
        char *buffer = malloc(bufferSize);
        snprintf(buffer, bufferSize, "%s: duplicated file", argv[idx]);
        reportError(report, buffer);
      } else {
        list->decls =
            realloc(list->decls, ++list->numDecl * sizeof(char const *));
        list->decls[list->numDecl - 1] = argv[idx];
      }
    } else {
      size_t bufferSize = 24 + length;
      char *buffer = malloc(bufferSize);
      snprintf(buffer, bufferSize, "%s: unrecogized extension", argv[idx]);
      reportError(report, buffer);
    }
  }

  return list;
}
// dtor
void fileListDestroy(FileList *list) {
  free(list->decls);
  free(list->codes);
  free(list);
}