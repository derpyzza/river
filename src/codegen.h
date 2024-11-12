#pragma once

#include <stdio.h>
struct Node;

/**
  takes in a file pointer and an AST.
  walks the AST, decodes it, and prints it out to the file
*/
void codegen(FILE* file, struct Node *tree);
