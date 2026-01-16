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

    result = merge(result, tmp);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    *offset = *i + 1;

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

    while (copy[i] != '\0')
    {
        if (copy[i] == '\'')
        {
            result = expand_single_quote(result, copy, &offset, &i);
            if (!result)
                return NULL;
        }
        else if (copy[i] == '\\')
        {
            result = expand_escape(result, copy, &offset, &i);
            if (!result)
                return NULL;
        }

        i++;
    }

    if (offset != i)
        result = middle_merge(result, copy, offset, i - offset);

    if (!result)
        return NULL;

    free(copy);

    return result;
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

    while (copy[i])
    {
        if (copy[i] == '\\')
        {
            if (offset != i)
                result = middle_merge(result, copy, offset, i - offset);

            if (!result)
                return NULL;

            char *tmp = calloc(3, sizeof(char));
            if (!tmp)
            {
                free(copy);
                free(result);
                return NULL;
            }

            i++;
            switch (copy[i])
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
                tmp[1] = copy[i];
                break;
            }

            offset = i + 1;
            result = merge(result, tmp);
            if (!result)
                return NULL;
        }

        i++;
    }

    if (offset != i)
        result = middle_merge(result, copy, offset, i - offset);

    if (!result)
        return NULL;

    free(copy);

    return result;
}
