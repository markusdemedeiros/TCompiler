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

// Compiles code modules into assembly files, guided by decl modules

#include "lexer/lexDump.h"
#include "parser/parser.h"
#include "util/errorReport.h"
#include "util/fileList.h"
#include "util/options.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  Report *report = reportCreate();

  // Read the given options, and validate them
  Options *options =
      parseOptions(report, (size_t)argc, (char const *const *)argv);
  if (reportState(report) == RPT_ERR) {
    reportDisplay(report);

    optionsDestroy(options);
    reportDestroy(report);
    return EXIT_FAILURE;
  }

  // Sort the given files, and validate them
  FileList *files =
      parseFiles(report, options, (size_t)argc, (char const *const *)argv);
  if (reportState(report) == RPT_ERR) {
    reportDisplay(report);

    fileListDestroy(files);
    optionsDestroy(options);
    reportDestroy(report);
    return EXIT_FAILURE;
  }

  // debug stop for lex
  if (optionsGet(options, optionDebugDump) == O_DD_LEX) {
    Report *dumpReport = reportCreate();
    lexDump(dumpReport, files);
    reportDisplay(dumpReport);
    reportDestroy(dumpReport);
  }

  // Parse the files
  // SOME_TYPE parsed = parse(report, options, files);

  // clean up
  fileListDestroy(files);
  optionsDestroy(options);
  reportDestroy(report);

  return EXIT_SUCCESS;
}