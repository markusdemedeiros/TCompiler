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

// Tests for the file list

#include "util/fileList.h"

#include "unitTests/tests.h"

#include <stdlib.h>
#include <string.h>

void fileListTest(TestStatus *status) {
  Report report;
  reportInit(&report);
  Options options;
  parseOptions(&options, &report, 1, NULL);
  char const *good[] = {"./tlc", "foo.tc", "bar.td"};
  char const *badExt[] = {"./tlc", "bad.badext", "good.tc"};
  char const *badDup[] = {"./tlc", "dup.tc", "dup.tc"};
  FileList fileList;

  // ctor
  parseFiles(&fileList, &report, &options, 1, NULL);
  dropLine();
  test(status, "[util] [fileList] [ctor] empty list produces no code files",
       fileList.codes.size == 0);
  test(status, "[util] [fileList] [ctor] empty list produces no decl files",
       fileList.decls.size == 0);
  test(status, "[util] [fileList] [ctor] no code files is caught",
       reportState(&report) == RPT_ERR);
  fileListUninit(&fileList);

  reportUninit(&report);
  reportInit(&report);

  parseFiles(&fileList, &report, &options, 3, good);
  test(status, "[util] [fileList] [ctor] code file is parsed as code file",
       fileList.codes.elements[0] == good[1]);
  test(status, "[util] [fileList] [ctor] decl file is parsed as decl file",
       fileList.decls.elements[0] == good[2]);
  fileListUninit(&fileList);

  reportUninit(&report);
  reportInit(&report);

  parseFiles(&fileList, &report, &options, 3, badExt);
  dropLine();
  test(status, "[util] [fileList] [ctor] badExt is caught",
       reportState(&report) == RPT_ERR);
  test(status, "[util] [fileList] [ctor] further files are processed correctly",
       fileList.codes.elements[0] == badExt[2]);
  fileListUninit(&fileList);

  reportUninit(&report);
  reportInit(&report);

  parseFiles(&fileList, &report, &options, 3, badDup);
  dropLine();
  test(status, "[util] [fileList] [ctor] duplicate is caught",
       reportState(&report) == RPT_ERR);
  test(status, "[util] [fileList] [ctor] first file is processed correctly",
       fileList.codes.elements[0] == badDup[1]);
  fileListUninit(&fileList);

  optionsUninit(&options);
  reportUninit(&report);
}