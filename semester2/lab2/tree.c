#include "tree.h"

#define INITIAL_STACK_SIZE 10

void print_dir_name(const char *dir_name, Stack *dir_stack, Stack *indent_stack) {
    const char *indent = pull(indent_stack);
    if (!dir_stack->length || strcmp(top(dir_stack), "..") == 0) {
        printf("%s└──%s/\n", indent, dir_name);
    } else {
        printf("%s├──%s/\n", indent, dir_name);
    }
}

int print_dir_content(Stack *dir_stack, Stack *indent_stack, const char *indent) {
    int err_code = OK;
    char *work_path = malloc(PATH_MAX * sizeof(char));
    if (!work_path) {
        return ERR_MALLOC;
    }
    getcwd(work_path, PATH_MAX);

    DIR *dp = NULL;
    if (!(dp = opendir(work_path))) {
        printf("Can't open directory %s, private directory\n", work_path);
        err_code = ERR_OPEN_DIR;
    } else {
        int was_pushed = 0;
        struct dirent *dirp;
        while ((dirp = readdir(dp))) {
            if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
                if (dirp->d_type == DT_DIR) {
                    push(indent_stack, strcat(strdup(indent), was_pushed ? "│\t" : "\t"));
                    push(indent_stack, indent);
                    push(dir_stack, dirp->d_name);
                    was_pushed = 1;
                } else {
                    printf("%s├──%s\n", indent, dirp->d_name);
                }
            }
        }
    }

    if (closedir(dp) < 0) {
        printf("Can't close directory %s\n", work_path);
        err_code = ERR_CLOSE_DIR;
    }

    return err_code;
}

int file_tree(const char *root) {
    int err_code = OK;

    Stack *dir_stack = init_stack(INITIAL_STACK_SIZE);
    push(dir_stack, root);
    Stack *indent_stack = init_stack(INITIAL_STACK_SIZE);
    push(indent_stack, "\t");
    push(indent_stack, "");

    while (dir_stack->length && !err_code) {
        const char *path = pull(dir_stack);

        if (strcmp(path, "..") == 0) {
            chdir("..");
        } else {
            print_dir_name(path, dir_stack, indent_stack);
            const char *indent = pull(indent_stack);

            push(dir_stack, "..");
            chdir(path);

            err_code = print_dir_content(dir_stack, indent_stack, indent);
        }
    }

    free_stack(dir_stack);
    free_stack(indent_stack);

    return err_code;
}
