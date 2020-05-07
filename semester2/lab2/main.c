#include "tree.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./tree \"root_path\"\n");
        return -1;
    }

    return file_tree(argv[1]);
}
