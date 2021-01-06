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
// the T Language Compiler. If not see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parser/functionBody.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileList.h"
#include "parser/common.h"

// TODO: also need to link ids to their referenced things
// TODO: remember to replace token strings with null if the string is used

// token stuff

/**
 * grabs the next token from unparsed
 *
 * like lex, but reads tokens from unparsed nodes
 *
 * assumes you don't go past the end of the unparsed
 *
 * @param unparsed node to read from
 * @param t token to write into
 */
static void next(Node *unparsed, Token *t) {
  // get pointer to saved token
  Token *saved =
      unparsed->data.unparsed.tokens->elements[unparsed->data.unparsed.curr];
  // give ownership of it to the caller
  memcpy(t, saved, sizeof(Token));
  // free malloc'ed block - this token has its guts ripped out
  free(saved);
  // clear it - we don't need to remember it any more
  unparsed->data.unparsed.tokens->elements[unparsed->data.unparsed.curr] = NULL;
  unparsed->data.unparsed.curr++;
}
static void prev(Node *unparsed, Token *t) {
  // malloc new block
  Token *saved = malloc(sizeof(Token));
  // copy from t (t now has its guts ripped out)
  memcpy(saved, t, sizeof(Token));
  // save it
  unparsed->data.unparsed.tokens->elements[--unparsed->data.unparsed.curr] =
      saved;
}

// miscellaneous functions

/**
 * skips tokens until an end of stmt is encountered
 *
 * consumes semicolons, leaves start of stmt tokens (including any ids) and
 * left/right braces (components of a compoundStmt individually fail and panic,
 * but a compoundStmt never ends with a panic)
 *
 * @param unparsed unparsed node to read from
 */
static void panicStmt(Node *unparsed) {
  while (true) {
    Token token;
    next(unparsed, &token);
    switch (token.type) {
      case TT_SEMI: {
        return;
      }
      case TT_LBRACE:
      case TT_RBRACE:
      case TT_IF:
      case TT_WHILE:
      case TT_DO:
      case TT_FOR:
      case TT_SWITCH:
      case TT_BREAK:
      case TT_CONTINUE:
      case TT_RETURN:
      case TT_ASM:
      case TT_VOID:
      case TT_UBYTE:
      case TT_CHAR:
      case TT_USHORT:
      case TT_UINT:
      case TT_INT:
      case TT_WCHAR:
      case TT_ULONG:
      case TT_LONG:
      case TT_FLOAT:
      case TT_DOUBLE:
      case TT_BOOL:
      case TT_ID:
      case TT_OPAQUE:
      case TT_STRUCT:
      case TT_UNION:
      case TT_ENUM:
      case TT_TYPEDEF: {
        prev(unparsed, &token);
        return;
      }
      default: {
        tokenUninit(&token);
        break;
      }
    }
  }
}

// context ignorant parsers

/**
 * parses an expression
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 *
 * @returns node or null on error
 */
static Node *parseExpression(FileListEntry *entry, Node *unparsed,
                             Environment *env) {
  return NULL;  // TODO: write this
}

// context sensitive parsers

static Node *parseStmt(FileListEntry *, Node *, Environment *);
/**
 * parses an if statement
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 * @param start first token
 *
 * @returns node or null on error
 */
static Node *parseIfStmt(FileListEntry *entry, Node *unparsed, Environment *env,
                         Token *start) {
  Token lparen;
  next(unparsed, &lparen);
  if (lparen.type != TT_LPAREN) {
    errorExpectedToken(entry, TT_LPAREN, &lparen);

    prev(unparsed, &lparen);
    panicStmt(unparsed);
    return NULL;
  }

  Node *predicate = parseExpression(entry, unparsed, env);
  if (predicate == NULL) {
    panicStmt(unparsed);
    return NULL;
  }

  Token rparen;
  next(unparsed, &rparen);
  if (rparen.type != TT_RPAREN) {
    errorExpectedToken(entry, TT_RPAREN, &rparen);

    prev(unparsed, &rparen);
    panicStmt(unparsed);

    nodeFree(predicate);
    return NULL;
  }

  environmentPush(env, hashMapCreate());
  Node *consequent = parseStmt(entry, unparsed, env);
  HashMap *consequentStab = environmentPop(env);
  if (consequent == NULL) {
    stabFree(consequentStab);
    nodeFree(predicate);
    return NULL;
  }

  Token elseKwd;
  next(unparsed, &elseKwd);
  if (elseKwd.type != TT_ELSE)
    return ifStmtNodeCreate(start, predicate, consequent, consequentStab, NULL,
                            NULL);

  environmentPush(env, hashMapCreate());
  Node *alternative = parseStmt(entry, unparsed, env);
  HashMap *alternativeStab = environmentPop(env);
  if (alternative == NULL) {
    stabFree(alternativeStab);
    nodeFree(consequent);
    stabFree(consequentStab);
    nodeFree(predicate);
    return NULL;
  }

  return ifStmtNodeCreate(start, predicate, consequent, consequentStab,
                          alternative, alternativeStab);
}

/**
 * parses a while statement
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 * @param start first token
 *
 * @returns node or null on error
 */
static Node *parseWhileStmt(FileListEntry *entry, Node *unparsed,
                            Environment *env, Token *start) {
  Token lparen;
  next(unparsed, &lparen);
  if (lparen.type != TT_LPAREN) {
    errorExpectedToken(entry, TT_LPAREN, &lparen);

    prev(unparsed, &lparen);
    panicStmt(unparsed);
    return NULL;
  }

  Node *condition = parseExpression(entry, unparsed, env);
  if (condition == NULL) {
    panicStmt(unparsed);
    return NULL;
  }

  Token rparen;
  next(unparsed, &rparen);
  if (rparen.type != TT_RPAREN) {
    errorExpectedToken(entry, TT_RPAREN, &rparen);

    prev(unparsed, &rparen);
    panicStmt(unparsed);

    nodeFree(condition);
    return NULL;
  }

  environmentPush(env, hashMapCreate());
  Node *body = parseStmt(entry, unparsed, env);
  HashMap *bodyStab = environmentPop(env);
  if (body == NULL) {
    stabFree(bodyStab);
    nodeFree(condition);
    return NULL;
  }

  return whileStmtNodeCreate(start, condition, body, bodyStab);
}

/**
 * parses a do-while statement
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 * @param start first token
 *
 * @returns node or null on error
 */
static Node *parseDoWhileStmt(FileListEntry *entry, Node *unparsed,
                              Environment *env, Token *start) {
  environmentPush(env, hashMapCreate());
  Node *body = parseStmt(entry, unparsed, env);
  HashMap *bodyStab = environmentPop(env);
  if (body == NULL) {
    panicStmt(unparsed);

    stabFree(bodyStab);
    return NULL;
  }

  Token whileKwd;
  next(unparsed, &whileKwd);
  if (whileKwd.type != TT_WHILE) {
    errorExpectedToken(entry, TT_WHILE, &whileKwd);

    prev(unparsed, &whileKwd);
    panicStmt(unparsed);

    nodeFree(body);
    stabFree(bodyStab);
    return NULL;
  }

  Token lparen;
  next(unparsed, &lparen);
  if (lparen.type != TT_LPAREN) {
    errorExpectedToken(entry, TT_LPAREN, &lparen);

    prev(unparsed, &lparen);
    panicStmt(unparsed);

    nodeFree(body);
    stabFree(bodyStab);
    return NULL;
  }

  Node *condition = parseExpression(entry, unparsed, env);
  if (condition == NULL) {
    panicStmt(unparsed);

    nodeFree(body);
    stabFree(bodyStab);
    return NULL;
  }

  Token rparen;
  next(unparsed, &rparen);
  if (rparen.type != TT_RPAREN) {
    errorExpectedToken(entry, TT_RPAREN, &rparen);

    prev(unparsed, &rparen);
    panicStmt(unparsed);

    nodeFree(condition);
    nodeFree(body);
    stabFree(bodyStab);
    return NULL;
  }

  return doWhileStmtNodeCreate(start, body, bodyStab, condition);
}

/**
 * parses a for statement initializer
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 *
 * @returns node or null on error
 */
static Node *parseForInitStmt(FileListEntry *entry, Node *unparsed,
                              Environment *env) {
  return NULL;  // TODO: write this
}

/**
 * parses a for statement
 *
 * @param entry entry containing this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 * @param start first token
 *
 * @returns node or null on error
 */
static Node *parseForStmt(FileListEntry *entry, Node *unparsed,
                          Environment *env, Token *start) {
  Token lparen;
  next(unparsed, &lparen);
  if (lparen.type != TT_LPAREN) {
    errorExpectedToken(entry, TT_LPAREN, &lparen);

    prev(unparsed, &lparen);
    panicStmt(unparsed);
    return NULL;
  }

  environmentPush(env, hashMapCreate());
  Node *initializer = parseForInitStmt(entry, unparsed, env);
  if (initializer == NULL) {
    panicStmt(unparsed);

    stabFree(environmentPop(env));
    return NULL;
  }

  Node *condition = parseExpression(entry, unparsed, env);
  if (condition == NULL) {
    panicStmt(unparsed);

    nodeFree(initializer);
    stabFree(environmentPop(env));
  }

  Token semi;
  next(unparsed, &semi);
  if (semi.type != TT_SEMI) {
    errorExpectedToken(entry, TT_SEMI, &semi);

    prev(unparsed, &semi);
    panicStmt(unparsed);

    nodeFree(condition);
    nodeFree(initializer);
    stabFree(environmentPop(env));
  }

  Token peek;
  next(unparsed, &peek);
  Node *increment = NULL;
  if (peek.type != TT_RPAREN) {
    // increment isn't null
    prev(unparsed, &peek);
    increment = parseExpression(entry, unparsed, env);
    if (increment == NULL) {
      panicStmt(unparsed);

      nodeFree(condition);
      nodeFree(initializer);
      stabFree(environmentPop(env));
      return NULL;
    }
  }

  Token rparen;
  next(unparsed, &rparen);
  if (rparen.type != TT_RPAREN) {
    errorExpectedToken(entry, TT_RPAREN, &rparen);

    prev(unparsed, &rparen);
    panicStmt(unparsed);

    nodeFree(increment);
    nodeFree(condition);
    nodeFree(initializer);
    stabFree(environmentPop(env));
    return NULL;
  }

  environmentPush(env, hashMapCreate());
  Node *body = parseStmt(entry, unparsed, env);
  HashMap *bodyStab = environmentPop(env);
  if (body == NULL) {
    nodeFree(increment);
    nodeFree(condition);
    nodeFree(initializer);
    stabFree(environmentPop(env));
    return NULL;
  }

  HashMap *loopStab = environmentPop(env);
  return forStmtNodeCreate(start, loopStab, initializer, condition, increment,
                           body, bodyStab);
}

static Node *parseCompoundStmt(FileListEntry *, Node *, Environment *);
/**
 * parses a statement
 *
 * @param entry entry that contains this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 *
 * @returns node or null on an error
 */
static Node *parseStmt(FileListEntry *entry, Node *unparsed, Environment *env) {
  Token peek;
  next(unparsed, &peek);
  switch (peek.type) {
    case TT_LBRACE: {
      // another compoundStmt
      prev(unparsed, &peek);
      return parseCompoundStmt(entry, unparsed, env);
    }
    case TT_IF: {
      return parseIfStmt(entry, unparsed, env, &peek);
    }
    case TT_WHILE: {
      return parseWhileStmt(entry, unparsed, env, &peek);
    }
    case TT_DO: {
      return parseDoWhileStmt(entry, unparsed, env, &peek);
    }
    case TT_FOR: {
      return parseForStmt(entry, unparsed, env, &peek);
    }
    case TT_SWITCH: {
      // TODO: switch
      return NULL;
    }
    case TT_BREAK: {
      // TODO: break
      return NULL;
    }
    case TT_CONTINUE: {
      // TODO: continue
      return NULL;
    }
    case TT_RETURN: {
      // TODO: return
      return NULL;
    }
    case TT_ASM: {
      // TODO: asm
      return NULL;
    }
    case TT_VOID:
    case TT_UBYTE:
    case TT_CHAR:
    case TT_USHORT:
    case TT_UINT:
    case TT_INT:
    case TT_WCHAR:
    case TT_ULONG:
    case TT_LONG:
    case TT_FLOAT:
    case TT_DOUBLE:
    case TT_BOOL: {
      // unambiguously a varDefn
      // TODO
      return NULL;
    }
    case TT_ID: {
      // maybe varDefn, maybe expressionStmt - disambiguate
      // TODO
      return NULL;
    }
    case TT_STAR:
    case TT_AMP:
    case TT_INC:
    case TT_DEC:
    case TT_MINUS:
    case TT_BANG:
    case TT_TILDE:
    case TT_CAST:
    case TT_SIZEOF:
    case TT_LPAREN:
    case TT_LSQUARE: {
      // unambiguously an expressionStmt
      // TODO
      return NULL;
    }
    case TT_OPAQUE: {
      // TODO: opaque
      // TODO: include semantics for opaque in functions in standard
      return NULL;
    }
    case TT_STRUCT: {
      // TODO: struct
      return NULL;
    }
    case TT_UNION: {
      // TODO: union
      return NULL;
    }
    case TT_ENUM: {
      // TODO: enum
      return NULL;
    }
    case TT_TYPEDEF: {
      // TODO: typedef
      return NULL;
    }
    case TT_SEMI: {
      return nullStmtNodeCreate(&peek);
    }
    default: {
      // unexpected token
      errorExpectedString(entry, "a declaration or a statement", &peek);

      prev(unparsed, &peek);

      panicStmt(unparsed);
      return NULL;
    }
  }
}

/**
 * parses a compound stmt
 *
 * @param entry entry that contains this node
 * @param unparsed unparsed node to read from
 * @param env environment to use
 *
 * @returns node - never null
 */
static Node *parseCompoundStmt(FileListEntry *entry, Node *unparsed,
                               Environment *env) {
  Token lbrace;
  next(unparsed, &lbrace);

  Vector *stmts = vectorCreate();
  environmentPush(env, hashMapCreate());

  while (true) {
    Token peek;
    next(unparsed, &peek);
    switch (peek.type) {
      case TT_RBRACE: {
        return compoundStmtNodeCreate(&lbrace, stmts, environmentPop(env));
      }
      case TT_EOF: {
        fprintf(stderr, "%s:%zu:%zu: error: unmatched left brace\n",
                entry->inputFilename, lbrace.line, lbrace.character);
        entry->errored = true;

        prev(unparsed, &peek);

        return compoundStmtNodeCreate(&lbrace, stmts, environmentPop(env));
      }
      default: {
        prev(unparsed, &peek);
        Node *stmt = parseStmt(entry, unparsed, env);
        if (stmt != NULL) vectorInsert(stmts, stmt);
        break;
      }
    }
  }
}

void parseFunctionBody(FileListEntry *entry) {
  Environment env;
  environmentInit(&env, entry);

  for (size_t bodyIdx = 0; bodyIdx < entry->ast->data.file.bodies->size;
       ++bodyIdx) {
    // for each top level thing
    Node *body = entry->ast->data.file.bodies->elements[bodyIdx];
    switch (body->type) {
      // if it's a funDefn
      case NT_FUNDEFN: {
        // setup stab for arguments
        HashMap *stab = hashMapCreate();
        environmentPush(&env, stab);

        for (size_t argIdx = 0; argIdx < body->data.funDefn.argTypes->size;
             ++argIdx) {
          Node *argType = body->data.funDefn.argTypes->elements[argIdx];
          Node *argName = body->data.funDefn.argNames->elements[argIdx];
          SymbolTableEntry *stabEntry =
              variableStabEntryCreate(entry, argType->line, argType->character);
          stabEntry->data.variable.type = nodeToType(argType, &env);
          if (stabEntry->data.variable.type == NULL) entry->errored = true;
          hashMapPut(stab, argName->data.id.id, stabEntry);
        }

        // parse and reference resolve body, replacing it in the original ast
        Node *unparsed = body->data.funDefn.body;
        body->data.funDefn.body = parseCompoundStmt(entry, unparsed, &env);
        nodeFree(unparsed);

        body->data.funDefn.argStab = environmentPop(&env);
        break;
      }
      default: {
        // don't need to do anything
        break;
      }
    }
  }

  environmentUninit(&env);
}