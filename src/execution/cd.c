#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "builtin.h"

static int go_to_root(char *path)
{
    char *new_old_path = getenv("PWD");
    if (!new_old_path)
    {
        warnx("go_to_root error during getenv PWD\n");
        return 1;
    }

    if (setenv("PWD", path, 1) == -1)
    {
        warnx("go_to_root error during PWD change to root dir\n");
        return 1;
    }

    if (setenv("OLDPWD", new_old_path, 1) == -1)
    {
        warnx("go_to_root error during OLDPWD change to new old path\n");
        return 1;
    }

    return 0;
}

char *give_begin(char *path)
{
    size_t i = 0;
    while (path[i] != '\0' && path[i] != '/')
        i++;

    return strndup(path, i);
}

static int is_back(char *path)
{
    size_t len = strlen(path);
    if (len >= 2 && path[0] == '.' && path[1] == '.')
    {
        if (path[2] == '\0')
            return 1;
        else if (len >= 3 && path[2] == '/')
            return 1;
    }

    return 0;
}

static char *go_back(char **new_path)
{
    size_t i = 0;
    int backslash = -1;
    while ((*new_path)[i] != '\0')
    {
        if ((*new_path)[i] == '/' && (*new_path)[i + 1] != '\0')
            backslash = i;

        i++;
    }

    if (backslash == -1)
    {
        free(*new_path);
        return strdup("");
    }
    else
    {
        char *res = strndup(*new_path, backslash);
        free(*new_path);
        return res;
    }
}

static char *merge(char *src1, char *src2)
{
    // Return NULL if one the parameters is NULL.
    if (!src1 || !src2)
    {
        free(src1);
        free(src2);
        return NULL;
    }

    // If one string is empty return the other.
    if (src1[0] == 0)
    {
        free(src1);
        return src2;
    }
    else if (src2[0] == 0)
    {
        free(src2);
        return src1;
    }

    // Get total length and resize.
    size_t len = strlen(src1) + strlen(src2) + 1;

    char *res = realloc(src1, len);
    if (!res)
    {
        free(src1);
        free(src2);
        return NULL;
    }

    // Merge strings
    res = strcat(res, src2);

    // Free other string
    free(src2);
    res[len - 1] = 0;

    return res;
}

static int go_to_dir(char *path)
{
    if (path[0] == '/')
    {
        return go_to_root(path);
    }

    char *new_old_path = getenv("PWD");
    if (!new_old_path)
    {
        warnx("go_to_dir error during getenv PWD\n");
        return 1;
    }

    char *new_path = strdup(new_old_path);
    char *next = give_begin(path);
    size_t len = strlen(next);

    while (strcmp(next, ""))
    {
        new_path = merge(new_path, strdup("/"));
        if (!new_path)
        {
            free(next);
            return 1;
        }

        if (is_back(next))
        {
            new_path = go_back(&new_path);
            free(next);
        }
        else
        {
            new_path = merge(new_path, next);
        }

        if (!new_path)
        {
            warnx("go_to_dir error during merge");
            return 1;
        }

        char *next = give_begin(path + len);
        len += strlen(next);
    }
    free(next);

    if (setenv("PWD", new_path, 1) == -1)
    {
        free(new_path);
        warnx("go_root_dir error during PWD change to root dir\n");
        return 1;
    }

    free(new_path);

    if (setenv("OLDPWD", new_old_path, 1) == -1)
    {
        warnx("go_root_dir error during OLDPWD change to new old path\n");
        return 1;
    }

    return 0;
}

static int switch_prev_act_dir(void)
{
    char *path = getenv("PWD");
    if (!path)
    {
        warnx("switch_prev_act_dir error during getenv PWD\n");
        return 1;
    }

    char *old_path = getenv("OLDPWD");
    if (!old_path)
    {
        warnx("switch_prev_act_dir error during getenv OLDPWD\n");
        return 1;
    }

    if (setenv("PWD", old_path, 1) == -1)
    {
        warnx("switch_prev_act_dir error during PWD change to old path\n");
        return 1;
    }

    if (setenv("OLDPWD", path, 1) == -1)
    {
        warnx("switch_prev_act_dir error during OLDPWD change to path\n");
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
        return go_to_root("~/");
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
        return go_to_root("~/");
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
