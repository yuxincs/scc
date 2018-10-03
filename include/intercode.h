#pragma once
#include <stdio.h>
#include "list.h"
#include "syntax.h"

/*
 * GOTO x       (GOTO  , , , x)
 * RETURN x     (RETURN, , , x)
 * ARG x        (ARG   , , , x)
 * PARAM x      (PARAM , , , x)
 * READ x       (READ  , , , x)
 * WRITE x      (WRITE , , , x)
 * LABEL x :    (LABEL, , , x)
 * FUNCTION f : (FUNCTION, , , f)
 * DEC x [size] (DEC, size, , x)
 * x := y       (:=, y , , x)
 * x := &y      (:=, &y, , x)
 * x := *y      (:=, *y, , x)
 * *x := y      (:=, y, , *x)
 * x := CALL f  (CALL, f, , x)
 * x := y + z   (+, y, z, x)
 * x := y - z   (-, y, z, x)
 * x := y * z   (*, y, z, x)
 * x := y / z   (/, y, z, x)
 * IF x [relop] y GOTO z  (<, x, y, z) (>, x, y, z) (Equal, x, y, z) (UnEqual, x, y, z) (LE, x, y, z) (GE, x, y, z)
 */

typedef enum Operator
{
     OP_GOTO,
     OP_RETURN,
     OP_ARG,
     OP_PARAM,
     OP_READ,
     OP_WRITE,
     OP_LABEL,
     OP_FUNCTION,
     OP_DEC,
     OP_DEFINE,
     OP_CALL,
     OP_PLUS,
     OP_MINUS,
     OP_MULTIPLY,
     OP_DIVIDE,
     OP_LESS,
     OP_GREATER,
     OP_EQUAL,
     OP_NOT_EQUAL,
     OP_LESS_OR_EQUAL,
     OP_GREATER_OR_EQUAL,
     OP_BITWISE_NEGATION,
     OP_LOGICAL_NEGATION
} Operator;

typedef struct Quad
{
    Operator op;
    char *arg1;
    char *arg2;
    char *result;
} Quad;

Quad *quad_new(Operator op);
void quad_delete(Quad *quad);
void print_quad_list(FILE *fp, List *code_list);

void generate_intermediate_code(List *code_list, Syntax *top_level);
