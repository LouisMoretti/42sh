#define _POSIX_C_SOURCE 200809L

#include "expansion/expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "config/config.h"
#include "utils/hash_map/hash_map.h"

#define HASH_MAP_SIZE 16

static struct hash_map *variables_hash_map = NULL;

struct hash_map *get_hm(void)
{
    return variables_hash_map;
}

int init_expansion(void)
{
    if (variables_hash_map != NULL)
    {
        warnx("init_expansion: Try to initialize an already initialised module "
              "!!!");
        return 1;
    }

    variables_hash_map = hash_map_init(HASH_MAP_SIZE);

    return 0;
}

void reset_expansion(void)
{
    hash_map_free(variables_hash_map);
    variables_hash_map = NULL;
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

static char *middle_merge(char *result, char *copy, size_t offset, size_t len)
{
    // Copy original string at the offset.
    char *original = strndup(copy + offset, len);
    if (!original)
    {
        free(result);
        free(copy);
        return NULL;
    }

    // Add it at the end of result.
    result = merge(result, original);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    return result;
}

static char *expand_single_quote(char *result, char *copy, size_t *offset,
                                 size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    // Skip first quote.
    (*i)++;
    *offset = *i;

    // Count quoted characters.
    size_t length = 0;
    while (copy[*i + length] && copy[*i + length] != '\'')
        length++;

    // Check for missing quote error.
    if (copy[*i + length] == '\0')
    {
        free(copy);
        free(result);
        return NULL;
    }

    // Make the substring of the quoted characters.
    char *quoted = strndup(copy + *offset, length);
    if (!quoted)
    {
        free(copy);
        free(result);
        return NULL;
    }

    // Add the substring to the result.
    result = merge(result, quoted);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    *i += length;
    // *offset += length + 1;
    *offset = *i + 1;

    return result;
}

static char *expand_escape(char *result, char *copy, size_t *offset, size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    // Go to escaped character.
    (*i)++;

    // New string with only the escaped character if it's a $, `, \ or ".
    char *tmp = calloc(3, sizeof(char));
    if (copy[*i] == '$' || copy[*i] == '`' || copy[*i] == '\\'
        || copy[*i] == '"')
        tmp[0] = copy[*i];
    else
    {
        // keeps the original string
        tmp[0] = '\\';
        tmp[1] = copy[*i];
    }
    // tmp[1] is already '\0'.

    // Add the escaped character to result.
    result = merge(result, tmp);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    *offset = *i + 1;

    return result;
}

static char *get_nb_args(char *res, struct config *my_conf)
{
    int nb_args = my_conf->arg_count - 1;
    int save_nb = nb_args;
    int size = nb_args == 0;
    while (nb_args > 0)
    {
        size++;
        nb_args /= 10;
    }
    // maybe use short of moulinette problem
    char *count_text = calloc(size + 1, sizeof(char));
    sprintf(count_text, "%i", save_nb); // > 127 arg = error
    free(res);
    return count_text;
}

static char *get_code(char *res, struct config *my_conf)
{
    int code = my_conf->previous_code;
    int save_code = code;
    int size = code == 0;
    while (code > 0)
    {
        size++;
        code /= 10;
    }
    char *code_text = calloc(size + 1, sizeof(char));
    sprintf(code_text, "%i", save_code); // > 127 arg = error
    free(res);
    return code_text;
}

static char *get_pid(char *res)
{
    int pid = getpid();
    int save_pid = pid;
    int size = pid == 0;
    while (pid > 0)
    {
        size++;
        pid /= 10;
    }
    char *pid_text = calloc(size + 1, sizeof(char));
    sprintf(pid_text, "%i", save_pid); // > 127 arg = error
    free(res);
    return pid_text;
}

static char *star_at_variable(char *res, struct config *my_conf)
{
    int i = 1;
    if (my_conf->arg_count >= 2)
    {
        res = merge(res, strdup(my_conf->arg_values[i]));
        if (!res)
            return NULL;
        i++;
    }
    while (i < my_conf->arg_count)
    {
        char *space = strdup(" ");
        res = merge(res, space);
        if (!res)
            return NULL;
        res = merge(res, strdup(my_conf->arg_values[i]));
        if (!res)
            return NULL;
        i++;
    }

    return res;
}

static char *get_special(char *name_var, int *code)
{
    char *res = calloc(1, sizeof(char));
    struct config *my_conf = get_conf();
    if (isdigit(name_var[0]))
    {
        // loop to look if the variable is valid
        size_t i = 0;
        while (name_var[i] != '\0' && isdigit(name_var[i]))
        {
            i++;
        }
        if (name_var[i] != '\0')
        {
            free(res);
            *code = 1;
            return NULL;
        }
        int index = atoi(name_var);

        if (index < my_conf->arg_count)
        {
            char *tmp = strdup(my_conf->arg_values[index]);
            res = merge(res, tmp);
        }

        if (!res)
            return NULL;
    }
    else if (name_var[0] == '?')
        return get_code(res, my_conf);
    else if (name_var[0] == '$')
        return get_pid(res);
    else if (name_var[0] == '#')
        return get_nb_args(res, my_conf);
    else if (name_var[0] == '@' || name_var[0] == '*')
    {
        return star_at_variable(res, my_conf);
    }
    else if (name_var[0] == '\0')
        res = merge(res, strdup("$"));
    else
    {
        free(res);
        return NULL;
    }
    return res;
}

static char *get_uid(void)
{
    uid_t id = geteuid();
    uid_t save_id = id;
    int size = 0;
    while (id > 0)
    {
        size++;
        id /= 10;
    }
    char *res = calloc(size + 1, sizeof(char));
    sprintf(res, "%i", save_id);
    return res;
}

static char *get_random(void)
{
    int rand_nb = rand();
    int save_nb = rand_nb;
    int size = 0;
    while (rand_nb > 0)
    {
        size++;
        rand_nb /= 10;
    }
    char *res = calloc(size + 1, sizeof(char));
    sprintf(res, "%i", save_nb);
    return res;
}

static char *get_env_var(char *name_var)
{
    char *env_var = getenv(name_var);
    if (!env_var)
        return strdup("");
    return strdup(env_var);
}

static char *get_value_var(char *special, char *name_var)
{
    if (special)
    {
        return special;
    }
    else
    {
        if (!variables_hash_map)
        {
            warnx("get_value_var: Try to use an un-initialised variable !!!");
            return strdup("");
        }
        if (!strcmp(name_var, "RANDOM"))
            return get_random();
        if (!strcmp(name_var, "UID"))
            return get_uid();
        if (!strcmp(name_var, "OLDPWD") || !strcmp(name_var, "PWD")
            || !strcmp(name_var, "IFS"))
            return get_env_var(name_var);
        char *val_var = hash_map_get(variables_hash_map, name_var);
        if (!val_var)
            return strdup("");

        char *res_val = strdup(val_var);
        return res_val;
    }
}

static char *bad_variable(char *special, char *copy, char *result)
{
    free(special);
    free(copy);
    free(result);
    warnx("Bad Expansion");
    return NULL;
}

static char *bad_braquets(char *copy, char *result)
{
    warnx("Expected '}'");
    free(copy);
    free(result);
    return NULL;
}

static char *expand_var(char *result, char *copy, size_t *offset, size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    (*i)++; // Skipping $
    char is_looked = copy[*i];

    if (copy[*i] == '{')
    {
        is_looked = '}';
        (*i)++;
    }
    else if (isalpha(copy[*i]))
        is_looked = '\0'; // Special case we need to go until last nonalpha char

    *offset = *i;

    if (is_looked == '}')
    {
        while (copy[*i] && copy[*i] != is_looked)
            (*i)++;
        if (!copy[*i])
        {
            return bad_braquets(copy, result);
        }
    }
    else if (is_looked == '\0')
    {
        while (isalnum(copy[*i]) || copy[*i] == '_')
            (*i)++;
    }
    else
    {
        (*i)++;
    }

    char *name_var = strndup(copy + *offset, *i - *offset);
    if (!name_var)
        return NULL; // TODO free

    if (is_looked == '}')
        (*i) += 1;
    *offset = *i; // Skipping closing bracket if present
    int code = 0;
    char *special = get_special(name_var, &code);
    if (code != 0)
    {
        return bad_variable(special, copy, result);
    }

    char *val_var = get_value_var(special, name_var);
    free(name_var);
    result = merge(result, val_var);

    if (!result)
        return NULL;

    return result;
}

static char *expand_double_quote(char *result, char *copy, size_t *offset,
                                 size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    // Skip first double quote.
    (*i)++;
    *offset = *i;

    // go threw all the copy until we find a double quote
    while (copy[*i] && copy[*i] != '"')
    {
        if (copy[*i] == '$')
        {
            result = expand_var(result, copy, offset, i);
            // result and copy are free inside expand_single_quote
            if (!result)
                return NULL;
        }
        else if (copy[*i] == '\\')
        {
            result = expand_escape(result, copy, offset, i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
        }

        (*i)++;
    }

    // Check for missing quote error.
    if (copy[*i] == '\0')
    {
        free(copy);
        free(result);
        warnx("Expand_double_quote: Error during expansion of double quotes\n");
        return NULL;
    }

    // Make the substring of the quoted characters.
    char *quoted = strndup(copy + *offset, *i - *offset);
    if (!quoted)
    {
        free(copy);
        free(result);
        return NULL;
    }

    // Add the substring to the result.
    result = merge(result, quoted);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    (*i)++;
    *offset = *i;

    return result;
}

char *expand_string(char *string)
{
    // Copy original string.
    char *copy = strndup(string, strlen(string));
    if (!copy)
        return NULL;

    // Initialize result to empty string.
    char *result = calloc(1, sizeof(char));
    if (!result)
    {
        free(copy);
        return NULL;
    }

    size_t i = 0;
    size_t offset = 0;

    // Loop through the string to find single quotes or escaped characters.
    while (copy[i] != '\0')
    {
        if (copy[i] == '"')
        {
            result = expand_double_quote(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
            i--;
        }
        else if (copy[i] == '\'')
        {
            result = expand_single_quote(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
        }
        else if (copy[i] == '\\')
        {
            result = expand_escape(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
        }
        else if (copy[i] == '$')
        {
            result = expand_var(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
            i--;
        }
        i++;
    }

    // Add cached characters to result.
    if (offset != i)
        result = middle_merge(result, copy, offset, i - offset);

    if (!result)
        // result and copy are free inside middle_merge.
        return NULL;

    free(copy);

    return result;
}

static char *expand_echo_escape(char *copy, size_t *i)
{
    char *tmp = calloc(3, sizeof(char));
    if (!tmp)
    {
        free(copy);

        return NULL;
    }

    (*i)++;
    switch (copy[*i])
    {
    case 'n':
        tmp[0] = '\n';
        break;
    case 't':
        tmp[0] = '\t';
        break;
    case '\\':
        tmp[0] = '\\';
        break;
    case '\0':
        tmp[0] = '\\';
        (*i)--;
        break;
    default:
        tmp[0] = '\\';
        tmp[1] = copy[*i];
        break;
    }

    return tmp;
}

char *expand_echo(char *word)
{
    // Copy original string.
    char *copy = strndup(word, strlen(word));
    if (!copy)
        return NULL;

    // Initialize result to empty string.
    char *result = calloc(1, sizeof(char));
    if (!result)
    {
        free(copy);
        return NULL;
    }

    size_t i = 0;
    size_t offset = 0;

    // Loop through the string to find single quotes or escaped characters.
    while (copy[i] != '\0')
    {
        if (copy[i] == '\\')
        {
            // Add cached characters to result.
            if (offset != i)
                result = middle_merge(result, copy, offset, i - offset);

            if (!result)
                return NULL;

            char *tmp = expand_echo_escape(copy, &i);
            offset = i + 1;
            result = merge(result, tmp);
            if (!result)
                return NULL;
        }

        i++;
    }

    // Add cached characters to result.
    if (offset != i)
        result = middle_merge(result, copy, offset, i - offset);

    if (!result)
        // result and copy are free inside middle_merge.
        return NULL;

    free(copy);

    return result;
}

static int expand_list_args(char **result, struct ast_word_list **word)
{
    struct config *my_conf = get_conf();
    int i = 1;
    struct ast_word_list *act = *word;

    // go threw all the arguments to add them one by one (but not the last one
    // !)
    while (i < my_conf->arg_count)
    {
        // copy the config argument in case an error occurs in the merge
        char *tmp = strdup(my_conf->arg_values[i]);
        // add the argument at the end of the result
        *result = merge(*result, tmp);
        if (!result)
            return -1;

        // malloc the word list to add an element
        struct ast_word_list *new_word =
            (struct ast_word_list *)init_ast(AST_WORD_LIST);
        if (!new_word)
            return -1;

        new_word->word = *result;
        act->next = (struct ast *)new_word;
        act = new_word;

        // create a new result variable
        *result = calloc(1, sizeof(char));
        if (!*result)
            return -1;

        i++;
    }

    // add the last argument in the result variable to be able to keep it for
    // the end of the given string in expand_for
    char *tmp = strdup(my_conf->arg_values[i]);
    *result = merge(*result, tmp);
    if (!result)
        return -1;

    return 0;
}

static char *res_merge_for(char **result, char **copy, size_t *offset,
                           size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        *result = middle_merge(*result, *copy, *offset, *i - *offset);

    if (!(*result))
        // result and copy are free inside middle_merge.
        return NULL;

    free(*copy);
    return *result;
}

static void switch_word(struct ast_word_list **word, char **result,
                        struct ast **next)
{
    if ((*word)->word)
        free((*word)->word);

    (*word)->word = *result;
    (*word)->next = *next;
}

static struct ast_word_list *expand_word_for(struct ast_word_list *word)
{
    struct ast_word_list *res = word;
    struct ast *next = word->next;

    // Copy original string.
    char *copy = strndup(word->word, strlen(word->word));
    if (!copy)
        return NULL;

    // Initialize result to empty string. (used to put the string in the
    // final_res)
    char *result = calloc(1, sizeof(char));
    if (!result)
    {
        free(copy);
        return NULL;
    }

    size_t i = 0;
    size_t offset = 0;

    // Loop through the string to find single quotes or escaped characters.
    while (copy[i] != '\0')
    {
        if (copy[i] == '\'')
        {
            result = expand_single_quote(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
        }
        else if (copy[i] == '\\')
        {
            result = expand_escape(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
        }
        else if (copy[i] == '$')
        {
            if (copy[i + 1] == '@')
            {
                // call this function to add in the word list all the arguments
                // given in the list

                if (expand_list_args(&result, &word) == -1)
                {
                    // put the next element of the word to be able to free
                    // everything
                    word->next = next;
                    return NULL;
                }

                i++;
            }
            else
            {
                result = expand_var(result, copy, &offset, &i);
                if (!result)
                    // result and copy are free inside expand_escape.
                    return NULL;
            }
        }

        i++;
    }

    if (!res_merge_for(&result, &copy, &offset, &i))
        return NULL;

    switch_word(&word, &result, &next);

    return res;
}

struct ast_word_list *expand_for(struct ast_word_list *word)
{
    struct ast_word_list *res = word;
    struct ast_word_list *act = word;

    while (act)
    {
        struct ast_word_list *next = (struct ast_word_list *)act->next;
        if (!expand_word_for(act))
            return NULL;

        act = next;
    }

    return res;
}
