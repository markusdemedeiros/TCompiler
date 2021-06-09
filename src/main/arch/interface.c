// Copyright 2020-2021 Justin Hu
//
// This file is part of the T Language Compiler.
//
// The T Language Compiler is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// The T Language Compiler is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// the T Language Compiler. If not see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "arch/interface.h"

#include "options.h"
#include "util/internalError.h"
#include "x86_64-linux/abi.h"
#include "x86_64-linux/asm.h"

char const *localLabelFormat(void) {
  switch (options.arch) {
    case OPTION_A_X86_64_LINUX: {
      return X86_64_LINUX_LOCAL_LABEL_FORMAT;
    }
    default: {
      error(__FILE__, __LINE__, "unrecognized architecture");
    }
  }
}

char const *prettyPrintRegister(size_t reg) {
  switch (options.arch) {
    case OPTION_A_X86_64_LINUX: {
      return x86_64LinuxPrettyPrintRegister(reg);
    }
    default: {
      error(__FILE__, __LINE__, "unrecognized architecture");
    }
  }
}

IRBlock *generateFunctionEntry(SymbolTableEntry *entry,
                               size_t returnValueAddressTemp,
                               FileListEntry *file) {
  switch (options.arch) {
    case OPTION_A_X86_64_LINUX: {
      return x86_64LinuxGenerateFunctionEntry(entry, returnValueAddressTemp,
                                              file);
    }
    default: {
      error(__FILE__, __LINE__, "unrecognized architecture");
    }
  }
}
IRBlock *generateFunctionExit(SymbolTableEntry const *entry,
                              size_t returnValueAddressTemp,
                              size_t returnValueTemp, FileListEntry *file) {
  switch (options.arch) {
    case OPTION_A_X86_64_LINUX: {
      return x86_64LinuxGenerateFunctionExit(entry, returnValueAddressTemp,
                                             returnValueTemp, file);
    }
    default: {
      error(__FILE__, __LINE__, "unrecognized architecture");
    }
  }
}