#define _POSIX_C_SOURCE 200809L

#include "expansion/expansion.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

#include "config/config.h"
#include "utils/hash_map/hash_map.h"

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

    // New string with only the escaped character.
    char *tmp = calloc(2, sizeof(char));
    tmp[0] = copy[*i] != '\0' ? copy[*i] : '\\';
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

static char *get_special(char *name_var, int *code)
{
    char *res = calloc(1, sizeof(char));
    struct config *my_conf = get_conf();

    if (isdigit(name_var[0]))
    {
        int index;
        // loop to look if the variable is valid
        size_t i = 0;
        while (name_var[i] != '\0' && isdigit(name_var[i]))
        {
            i++;
        }
        if (name_var[i] != '\0')
            return NULL;
        index = atoi(name_var);
        res = merge(res, my_conf->arg_values[index]);
    }
    else if (name_var[0] == '?')
        res = merge(res, strdup("0")); // TODO
    else if (name_var[0] == '$')
        res = merge(res, strdup("$"));
    else if (name_var[0] == '#')
    {
        struct config *my_conf = get_conf();
        char *count_text = calloc(2, sizeof(char));
        sprintf(count_text, "%i", my_conf->arg_count);
    }

    else if (name_var[0] == '@')
    {
        int i = 0;
        if (my_conf->arg_count >= 1)
        {
            res = merge(res, my_conf->arg_values[i]);
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

            res = merge(res, my_conf->arg_values[i]);
            if (!res)
                return NULL;

            i++;
        }
    }
    else if (name_var[0] == '*')
    {
        // TODO
    }
    else
    {
        *(code) = 1;
        return NULL;
    }

    return res;
}

static char *expand_var(char *result, char *copy, size_t *offset, size_t *i)
{
    // Add cached characters to result.
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    (*i)++; // Skipping $
    char is_looked;

    if (copy[*i] == '{')
    {
        is_looked = '}';
        (*i)++;
    }
    else if (isalpha(copy[*i]))
        is_looked = '\0'; // Special case we need to go until last nonalpha char
    else if (isdigit(copy[*i]))
        is_looked = copy[*i + 1];
    else
        return NULL;

    *offset = *i;

    if (is_looked != '\0')
    {
        while (copy[*i] != is_looked)
            (*i)++;
    }
    else
    {
        while (isalnum(copy[*i]) || copy[*i] == '_')
            (*i)++;
    }

    char *name_var = strndup(copy + *offset, *i - *offset);
    if (!name_var)
        return NULL;

    int code = 0;
    char *special = get_special(name_var, &code);
    if (code != 0)
    {
        warnx("Bad Expansion");
        return NULL;
    }

    char *val_var = NULL;
    if (special)
    {
        val_var = special;
    }
    else
    {
        struct config *my_conf = get_conf();
        struct hash_map *my_variables = my_conf->hash_map_variables;
        val_var = hash_map_get(my_variables, name_var);
        if (!val_var)
            return strdup("");
    }

    free(name_var);
    result = merge(result, val_var);
    if (!result)
        return NULL;

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
        switch (copy[i])
        {
        case '\'':
            result = expand_single_quote(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
            break;
        case '\\':
            result = expand_escape(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
            break;
        case '$':
            result = expand_var(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
            break;
        default:
            break;
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

static char **free_list_args(char **final_res)
{
    if (final_res)
    {
        size_t i = 0;

        while (final_res[i])
        {
            free(final_res[i]);
            i++;
        }

        free(final_res);
    }

    return NULL;
}

static char **expand_list_args(char **result, char **final_res, size_t *len_res)
{
    struct config *my_conf = get_conf();
    int i = 0;

    // go threw all the arguments to add them one by one (but not the last one
    // !)
    while (i < my_conf->arg_count - 1)
    {
        // copy the config argument in case an error occurs in the merge
        char *tmp = strdup(my_conf->arg_values[i]);
        // add the argument at the end of the result
        *result = merge(*result, tmp);
        if (!result)
            return free_list_args(final_res);

        // realloc the final_res to be able to add one result inside
        final_res = realloc(final_res, (*len_res + 1) * sizeof(char *));
        if (!final_res)
        {
            free(*result);
            return NULL;
        }

        // add the result at the end of the final_res list
        final_res[*len_res - 1] = *result;
        final_res[*len_res] = NULL;
        (*len_res)++;

        // create a new result variable
        *result = calloc(1, sizeof(char));
        if (!*result)
        {
            return free_list_args(final_res);
        }
        i++;
    }

    // add the last argument in the result variable to be able to keep it for
    // the end of the given string in expand_for
    char *tmp = strdup(my_conf->arg_values[i]);
    *result = merge(*result, tmp);
    if (!result)
        return free_list_args(final_res);

    return final_res;
}

char **expand_for(char *string)
{
    // Copy original string.
    char *copy = strndup(string, strlen(string));
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

    // initialize the result of the function of length 1
    size_t len_res = 1;
    char **final_res = calloc(1, sizeof(char *));
    if (!final_res)
    {
        free(copy);
        free(result);
        return NULL;
    }

    size_t i = 0;
    size_t offset = 0;

    // Loop through the string to find single quotes or escaped characters.
    while (copy[i] != '\0')
    {
        switch (copy[i])
        {
        case '\'':
            result = expand_single_quote(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_single_quote.
                return NULL;
            break;
        case '\\':
            result = expand_escape(result, copy, &offset, &i);
            if (!result)
                // result and copy are free inside expand_escape.
                return NULL;
            break;
        case '$':
            if (copy[i + 1] == '@')
            {
                // call this function to add in the final_res all the arguments
                // given in the list
                char **final_res =
                    expand_list_args(&result, final_res, &len_res);
                if (!final_res)
                    return NULL;
                i++;
            }
            else
            {
                result = expand_var(result, copy, &offset, &i);
                if (!result)
                    // result and copy are free inside expand_escape.
                    return NULL;
            }
            break;
        default:
            break;
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

    // Add the last result in the final_result list
    final_res = realloc(final_res, (len_res + 1) * sizeof(char *));
    if (!final_res)
    {
        free(result);
        return NULL;
    }

    final_res[len_res - 1] = result;
    final_res[len_res] = NULL;

    return final_res;
}
