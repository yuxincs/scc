#ifndef SCC_SEMANTIC_HEADER
#define SCC_SEMANTIC_HEADER

#include <stdio.h>
#include <stdbool.h>
#include "syntax.h"
#include "symboltable.h"

bool semantic_analysis(Syntax * top_level);

#endif
