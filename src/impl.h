#pragma once

#include "derp.h"
#include "parser/parser.h"
#include "memory.h"

void eval_ast(Node* tree, d_arena mem);
