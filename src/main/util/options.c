// Copyright 2020 Justin Hu
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

#include "util/options.h"

#include <stdio.h>
#include <string.h>

Options options = {
    ARCH_OPTION_X86_64,   POSITION_DEPENDENCE_OPTION_PDC, WARNING_OPTION_ERROR,
    WARNING_OPTION_ERROR, DEBUG_DUMP_OPTION_NONE,
};

int parseArgs(size_t argc, char const *const *argv, size_t *numFilesOut) {
  size_t numFiles = 0;

  for (size_t idx = 1; idx < argc; idx++) {
    if (argv[idx][0] != '-') {
      // not an option
      numFiles++;
    } else if (strcmp(argv[idx], "--") == 0) {
      // remaining options are all files
      numFiles += argc - idx - 1;
      break;
    } else if (strcmp(argv[idx], "--arch=x86_64") == 0) {
      options.arch = ARCH_OPTION_X86_64;
    } else if (strcmp(argv[idx], "-fPDC") == 0) {
      options.positionDependence = POSITION_DEPENDENCE_OPTION_PDC;
    } else if (strcmp(argv[idx], "-fPIE") == 0) {
      options.positionDependence = POSITION_DEPENDENCE_OPTION_PIE;
    } else if (strcmp(argv[idx], "-fPIC") == 0) {
      options.positionDependence = POSITION_DEPENDENCE_OPTION_PIC;
    } else if (strcmp(argv[idx], "-Wduplicate-file=error") == 0) {
      options.duplicateFile = WARNING_OPTION_ERROR;
    } else if (strcmp(argv[idx], "-Wduplicate-file=warn") == 0) {
      options.duplicateFile = WARNING_OPTION_WARN;
    } else if (strcmp(argv[idx], "-Wduplicate-file=ignore") == 0) {
      options.duplicateFile = WARNING_OPTION_IGNORE;
    } else if (strcmp(argv[idx], "-Wunrecognized-file=error") == 0) {
      options.unrecognizedFile = WARNING_OPTION_ERROR;
    } else if (strcmp(argv[idx], "-Wunrecognized-file=warn") == 0) {
      options.unrecognizedFile = WARNING_OPTION_WARN;
    } else if (strcmp(argv[idx], "-Wunrecognized-file=ignore") == 0) {
      options.unrecognizedFile = WARNING_OPTION_IGNORE;
    } else if (strcmp(argv[idx], "--debug-dump=none") == 0) {
      options.dump = DEBUG_DUMP_OPTION_NONE;
    } else if (strcmp(argv[idx], "--debug-dump=lex") == 0) {
      options.dump = DEBUG_DUMP_OPTION_LEX;
    } else {
      fprintf(stderr, "tlc: error: options '%s' not recognized\n", argv[idx]);
      return -1;
    }
  }

  *numFilesOut = numFiles;

  return 0;
}