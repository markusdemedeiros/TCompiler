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

// Implementation of the test engine status object

#include "engine.h"

#include <stdlib.h>

TestStatus status;

void testStatusInit(void) {
  status.numTests = 0;
  status.numPassed = 0;
}
int testStatusStatus(void) {
  return status.numTests == status.numPassed ? 0 : -1;
}
void test(char const *name, bool condition) {
  if (condition) {
    status.numTests++;
    status.numPassed++;
  } else {
    printf("\x1B[1;91mFAILED: %s\x1B[m\n", name);
    status.numTests++;
  }
}
void dropLine(void) { fprintf(stderr, "\x1B[1A\x1B[2K"); }