#define _POSIX_C_SOURCE 200112L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "builtin.h"

static int go_to_dir(char *path)
{
    char *new_old_path = getenv("PWD");
    if (!new_old_path)
    {
        printf("go_root_dir error during getenv PWD\n");
        fflush(stdout);
        return 1;
    }

    if (setenv("PWD", path, 1) == -1)
    {
        printf("go_root_dir error during PWD change to root dir\n");
        fflush(stdout);
        return 1;
    }

    if (setenv("OLDPWD", new_old_path, 1) == -1)
    {
        printf("go_root_dir error during OLDPWD change to new old path\n");
        fflush(stdout);
        return 1;
    }

    return 0;
}

static int switch_prev_act_dir(void)
{
    char *path = getenv("PWD");
    if (!path)
    {
        printf("switch_prev_act_dir error during getenv PWD\n");
        fflush(stdout);
        return 1;
    }

    char *old_path = getenv("OLDPWD");
    if (!old_path)
    {
        printf("switch_prev_act_dir error during getenv OLDPWD\n");
        fflush(stdout);
        return 1;
    }

    if (setenv("PWD", old_path, 1) == -1)
    {
        printf("switch_prev_act_dir error during PWD change to old path\n");
        fflush(stdout);
        return 1;
    }

    if (setenv("OLDPWD", path, 1) == -1)
    {
        printf("switch_prev_act_dir error during OLDPWD change to path\n");
        fflush(stdout);
        return 1;
    }

    return 0;
}

static int change_dir(char *path)
{
    struct stat st;
    stat(path, &st);

    if (S_ISDIR(st.st_mode) == 1)
    {
        return go_to_dir(path);
    }
    else if (S_ISREG(st.st_mode) == 1)
    {
        printf("the given path is a file !\n");
        fflush(stdout);
        return 1;
    }
    else
    {
        printf("the given path does not exists !\n");
        fflush(stdout);
        return 1;
    }
}

int builtin_cd(struct ast_simple_cmd *command)
{
    struct ast_element_list *element_list =
        (struct ast_element_list *)command->element_list;

    if (!element_list)
    {
        return go_to_dir("~/");
    }

    if (element_list->next)
    {
        printf("Too many arguments\n");
        fflush(stdout);
        return 1;
    }

    // cd with one argument -> go to the directory given if it exists
    struct ast_element *first_element =
        (struct ast_element *)element_list->element;

    // cd with no argument -> go to the root directory
    if (!first_element)
    {
        return go_to_dir("~/");
    }
    else if (!strcmp(first_element->word, "-"))
    {
        return switch_prev_act_dir();
    }
    else
    {
        return change_dir(first_element->word);
    }
}
