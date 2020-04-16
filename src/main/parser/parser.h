// Copyright 2020 Justin Hu
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
// The T Language Compiler. If not, see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file
 * parser
 */

#ifndef TLC_PARSER_PARSER_H_
#define TLC_PARSER_PARSER_H_

/**
 * parses all of the files in the file list
 *
 * no lexer states or lexer tables may be initialized, and all file entry
 * programs must be null
 *
 * @returns status code (0 = OK, -1 = fatal error)
 */
int parse(void);

#endif  // TLC_PARSER_PARSER_H_