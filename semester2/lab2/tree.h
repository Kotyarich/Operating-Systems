#ifndef TREE_H
#define TREE_H

#include <dirent.h>
#include <unistd.h>
#include "stack.h"

#define ERR_OPEN_DIR -1
#define ERR_CLOSE_DIR -2
#define ERR_MALLOC -3
#define OK 0

int file_tree(const char *root);

#endif //TREE_H
