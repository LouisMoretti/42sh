#define _POSIX_C_SOURCE 200809L

#include "expansion/expansion.h"

#include <stdlib.h>
#include <string.h>

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

static char *expand_var(char *result, char *copy, size_t *offset, size_t *i)
{
    
    if (*offset != *i)
        result = middle_merge(result, copy, *offset, *i - *offset);

    if (!result)
        return NULL;

    (*i)++; // Skipping $

    char is_looked = ' ';
    if(copy[*i] == '{')
    {
        is_looked = '}'
        (*i)++;
    } 
    offset = *i;

    while (copy[*i] != is_looked)
        (*i)++;
    char *name_var = strndup(copy + *offset, *i - *offset);
    if(!name_var)
        return NULL;
    // hash_map_get -> val_var;
    free(name_var);
    result = merge(result, val_var);
    if(!result)
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
        switch(copy[i])
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

