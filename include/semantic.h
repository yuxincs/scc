#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "syntax.h"
#include "symboltable.h"

bool semantic_analysis(Syntax * top_level);
