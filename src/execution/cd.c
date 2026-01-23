#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtin.h"

// Function that will put the user to the path given.
static int go_to_path(char *path)
{
    // Keep the old path in memory.
    char *tmp = getenv("PWD");
    if (!tmp)
    {
        warnx("go_to_path: error during getenv PWD.");
        return 1;
    }
    char *new_old_path = strdup(tmp);

    // Set the new PWD env variable.
    if (setenv("PWD", path, 1) == -1)
    {
        free(new_old_path);
        warnx("go_to_path: error during PWD change to root dir.");

        return 1;
    }

    // Set the new OLDPWD env variable.
    if (setenv("OLDPWD", new_old_path, 1) == -1)
    {
        free(new_old_path);
        warnx("go_to_path: error during OLDPWD change to new old path.");

        return 1;
    }

    free(new_old_path);

    // Set the chdir variable to the path (to make the user really go in this
    // directory).
    if (chdir(path) == -1)
    {
        warnx("go_to_path: error during chdir.");
        return 1;
    }

    return 0;
}

// Gives the char * from the start to the next / or the end.
static char *give_begin(char *path, size_t *len)
{
    size_t i = *len;
    while (path[*len] != '\0' && path[*len] != '/')
        (*len)++;

    if (path[*len] == '/')
    {
        (*len)++;
        return strndup(path + i, *len - i - 1);
    }

    return strndup(path + i, *len - i);
}

// Check if the given path is .. or ../ (which will remove the last directory of
// the PWD).
static int is_back(char *path)
{
    size_t len = strlen(path);
    if (len >= 2 && path[0] == '.' && path[1] == '.'
        && (path[2] == '/' || path[2] == '\0'))
        return 1;

    return 0;
}

// Give the char * without the last directory for example : hello/there/coco
// will become hello/there.
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
        return calloc(1, sizeof(char));
    }
    else
    {
        char *res = strndup(*new_path, backslash);
        free(*new_path);
        return res;
    }
}

// Function that will merge src1 and src2 (src2 will be free).
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

    // Merge strings.
    res = strcat(res, src2);

    // Free other string.
    free(src2);
    res[len - 1] = 0;

    return res;
}

// Check if the given path is . or ./ (which will not change the path).
static int is_here(char *path)
{
    if (path[0] == '.' && (path[1] == '\0' || path[1] == '/'))
        return 1;

    return 0;
}

// Check if the given path is a directory or not.
static int check_dir(char *path)
{
    struct stat st;
    stat(path, &st);

    if (S_ISDIR(st.st_mode) == 1)
    {
        return 1;
    }
    else if (S_ISREG(st.st_mode) == 1)
    {
        warnx("The given path is a file !");
        return 0;
    }
    else
    {
        warnx("The given path does not exist !");
        return 0;
    }
}

static char *remove_last_backslash(char *new_path)
{
    char *res = strndup(new_path, strlen(new_path) - 1);

    free(new_path);

    if (!res)
        return NULL;

    return res;
}

// tmp is static and new_path is malloc
static int change_dir(char *tmp, char *new_path)
{
    // Check if the new path is the same than PWD (if it is, we're not doing
    // anything).
    char *new_old_path = strdup(tmp);
    if (strcmp(new_old_path, new_path) == 0)
    {
        free(new_path);
        free(new_old_path);
        return 0;
    }

    // We're setting all the env variable to the right thing.
    if (setenv("PWD", new_path, 1) == -1)
    {
        free(new_path);
        free(new_old_path);
        warnx("change_dir: error during PWD change to root dir.");
        return 1;
    }

    if (setenv("OLDPWD", new_old_path, 1) == -1)
    {
        free(new_path);
        free(new_old_path);
        warnx("change_dir: error during OLDPWD change to new old path.");
        return 1;
    }

    free(new_old_path);

    if (chdir(new_path) == -1)
    {
        free(new_path);
        warnx("change_dir: error during chdir.");
        return 1;
    }

    free(new_path);

    return 0;
}

// Function that will change the PWD, OLDPWD and chdir env variable to be good
// with the potential new path.
static int go_to_dir(char *path)
{
    // Check if the potential new path is good.
    if (check_dir(path) == 0)
    {
        return 1;
    }

    // Keep the PWD to put it in the OLDPWD later.
    char *tmp = getenv("PWD");
    if (!tmp)
    {
        warnx("go_to_dir: error during getenv PWD.");
        return 1;
    }

    char *new_path = strdup(tmp);
    size_t len = 0;

    // Next will take the next directory to change.
    char *next = give_begin(path, &len);

    // Loop to go threw the given path step by step (at each '/', we're stopping
    // and changing the new_path).
    while (next[0] != '\0')
    {
        // Add a / at the end to be able to add a directory.
        new_path = merge(new_path, strdup("/"));
        if (!new_path)
        {
            free(next);
            return 1;
        }

        // Change the new_path depending on what we have in the current next.
        if (is_back(next)) // Check if it a .. or ../
        {
            new_path = go_back(&new_path);
        }
        else if (!is_here(next)) // Check if it is a . or ./
        {
            new_path = merge(new_path, strdup(next));
        }
        else
        {
            new_path = remove_last_backslash(new_path);
        }

        free(next);

        // If the new_path is NULL -> an error occured.
        if (!new_path)
        {
            warnx("go_to_dir: error during merge.");
            return 1;
        }

        // We're taking the next directory to change.
        next = give_begin(path, &len);
    }
    free(next);

    return change_dir(tmp, new_path);
}

// This function just switches the PWD and OLDPWD env variable and changes the
// chdir env variable.
static int switch_prev_act_dir(void)
{
    char *tmp = getenv("PWD");
    if (!tmp)
    {
        warnx("switch_prev_act_dir: error during getenv PWD.");
        return 1;
    }

    char *path = strdup(tmp);

    tmp = getenv("OLDPWD");
    if (!tmp)
    {
        free(path);
        warnx("switch_prev_act_dir: error during getenv OLDPWD.");
        return 1;
    }

    char *old_path = strdup(tmp);

    if (setenv("PWD", old_path, 1) == -1)
    {
        free(path);
        free(old_path);
        warnx("switch_prev_act_dir: error during PWD change to old path.");
        return 1;
    }

    if (setenv("OLDPWD", path, 1) == -1)
    {
        free(path);
        free(old_path);
        warnx("switch_prev_act_dir: error during OLDPWD change to path.");
        return 1;
    }

    free(path);

    if (chdir(old_path) == -1)
    {
        free(old_path);
        warnx("switch_prev_act_dir: error during chdir.");
        return 1;
    }

    free(old_path);

    return 0;
}

// Changes the PWD to ~ + HOME + word (word is the direct path to a directory,
// like : /afs).
static int go_to_root_plus_dir(char *home, char *word)
{
    char *path = merge(strdup("~"), strdup(home));
    if (!path)
    {
        warnx("go_to_root_plus_dir: error during a merge or strdup in go to "
              "root dir.");
        return 1;
    }

    path = merge(path, strdup(word));
    if (!path)
    {
        warnx("go_to_root_plus_dir: error during a merge or strdup in go to "
              "root dir.");
        return 1;
    }

    if (check_dir(path) == 0)
    {
        free(path);
        return 1;
    }

    char *tmp = getenv("PWD");
    if (!tmp)
    {
        free(path);
        warnx("go_to_root_plus_dir: error during getenv PWD.");

        return 1;
    }

    char *old_path = strdup(tmp);

    if (setenv("PWD", path, 1) == -1)
    {
        free(path);
        free(old_path);
        warnx("go_to_root_plus_dir: error during PWD change to old path.");

        return 1;
    }

    free(path);

    if (setenv("OLDPWD", old_path, 1) == -1)
    {
        free(old_path);
        warnx("go_to_root_plus_dir: error during OLDPWD change to path.");

        return 1;
    }

    free(old_path);
    return 0;
}

int builtin_cd(struct ast_simple_cmd *command)
{
    struct ast_element_list *element_list =
        (struct ast_element_list *)command->element_list;

    // Check if the given cmd is 'cd' with no argument.
    if (!element_list)
    {
        char *home = getenv("HOME");
        if (!home || home[0] != '\0')
            return 0;

        return go_to_path(home);
    }

    // Only one argument allowed for cd.
    if (element_list->next)
    {
        warnx("Too many arguments for cd.");
        return 1;
    }

    // 'cd' with one argument -> go to the directory given if it exists.
    struct ast_element *first_element =
        (struct ast_element *)element_list->element;

    // 'cd' with no argument -> go to the root directory.
    if (!first_element)
    {
        char *home = getenv("HOME");
        if (!home || home[0] != '\0')
            return 0;

        return go_to_path(home);
    }
    else if (first_element->word[0] == '-') // cd - switches the OLDPWD and PWD.
    {
        return switch_prev_act_dir();
    }
    else
    {
        if (strcmp(first_element->word, getenv("PWD")) == 0)
            return 0;

        // Check if we have a direct changes.
        if (first_element->word[0] == '/')
        {
            char *home = getenv("HOME");
            if (!home || home[0] != '\0')
                home = "";

            return go_to_root_plus_dir(home, first_element->word);
        }

        return go_to_dir(first_element->word);
    }
}
