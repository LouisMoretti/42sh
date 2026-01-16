#define _POSIX_C_SOURCE 200809L

#include "expansion/expansion.h"

#include <stdlib.h>
#include <string.h>

static char *merge(char *src1, char *src2)
{
    // // Return NULL if one the parameters is NULL.
    // if (!src1)
    // {
    //     if (src2)
    //         free(src2);
    //     return NULL;
    // }
    // else if (!src2)
    // {
    //     if (src1)
    //         free(src1);
    //     return NULL;
    // }

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
    // realloc failed
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

static char *middle_merge(char *result, char *copy, size_t beg, size_t i)
{
    char *classic = strndup(copy + beg, i - beg);
    if (!classic)
    {
        free(result);
        free(copy);
        return NULL;
    }

    result = merge(result, classic);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    return result;
}

static char *expand_single_quote(char *result, char *copy, size_t *beg,
                                 size_t *i)
{
    if (*beg != *i)
        result = middle_merge(result, copy, *beg, *i);

    if (!result)
        return NULL;

    (*i)++;
    *beg = *i;
    while (copy[*i] && copy[*i] != '\'')
        (*i)++;

    if (!copy[*i])
    {
        free(copy);
        free(result);
        return NULL;
    }

    char *quoted = strndup(copy + *beg, *i - *beg);
    if (!quoted)
    {
        free(copy);
        free(result);
        return NULL;
    }

    result = merge(result, quoted);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    *beg = *i + 1;

    return result;
}

static char *expand_escape(char *result, char *copy, size_t *beg, size_t *i)
{
    if (*beg != *i)
        result = middle_merge(result, copy, *beg, *i);

    if (!result)
        return NULL;

    (*i)++;
    char escaped = copy[*i];

    char *tmp = strndup(&escaped, 1);
    result = merge(result, tmp);
    if (!result)
    {
        free(copy);
        return NULL;
    }

    *beg = *i + 1;

    return result;
}

char *expand_string(char *string)
{
    char *copy = strndup(string, strlen(string));
    if (!copy)
        return NULL;

    char *result = calloc(1, sizeof(char));
    if (!result)
    {
        free(copy);
        return NULL;
    }

    size_t i = 0;
    size_t beg = 0;

    while (copy[i])
    {
        if (copy[i] == '\'')
        {
            result = expand_single_quote(result, copy, &beg, &i);
            if (!result)
                return NULL;
        }
        else if (copy[i] == '\\')
        {
            result = expand_escape(result, copy, &beg, &i);
            if (!result)
                return NULL;
        }

        i++;
    }
    if (beg != i)
        result = middle_merge(result, copy, beg, i);
    if (!result)
        return NULL;

    free(copy);

    return result;
}

char *expand_echo(char *word)
{
    char *copy = strndup(word, strlen(word));
    if (!copy)
        return NULL;

    char *result = calloc(1, sizeof(char));
    if (!result)
    {
        free(copy);
        return NULL;
    }

    size_t i = 0;
    size_t beg = 0;

    while (copy[i])
    {
        if (copy[i] == '\\')
        {
            if (beg != i)
                result = middle_merge(result, copy, beg, i);
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

            beg = i + 1;
            result = merge(result, tmp);
            if (!result)
                return NULL;
        }

        i++;
    }

    if (beg != i)
        result = middle_merge(result, copy, beg, i);
    if (!result)
        return NULL;

    free(copy);

    return result;
}
