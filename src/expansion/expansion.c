#define _POSIX_C_SOURCE 200809L

#include "expansion/expansion.h"

#include <stdlib.h>
#include <string.h>

static char *merge(char *src1, char *src2)
{
    if (!src1)
    {
        if (src2)
            free(src2);
        return NULL;
    }
    else if (!src2)
    {
        free(src1);
        return NULL;
    }

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

    size_t len = strlen(src1) + strlen(src2) + 1;
    src1 = realloc(src1, len);
    // realloc failed
    if (!src1)
    {
        free(src2);
        return NULL;
    }
    // Merge strings
    src1 = strcat(src1, src2);
    // Free other string
    free(src2);
    src1[len - 1] = 0;

    return src1;
}

char *middle_merge(char *result, char *copy, size_t beg, size_t i)
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

char *expand_single_quote(char *result, char *copy, size_t *beg, size_t *i)
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
    result = merge(result, quoted);
    if (!result)
    {
        free(copy);

        return NULL;
    }

    *beg = *i + 1;

    return result;
}

char *expand_escape(char *result, char *copy, size_t *beg, size_t *i)
{
    if (*beg != *i)
        result = middle_merge(result, copy, *beg, *i);
    if (!result)
        return NULL;

    (*i)++;
    char escaped = copy[*i];
    if (escaped == 'n')
        escaped = '\n';
    else if (escaped == 't')
        escaped = '\t';

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

char *quote_removal(char *string)
{
    char *res = calloc(1, sizeof(char));
    if (!res)
	return NULL;

    char *copy = strndup(string);
    if(!copy)
    {
	free(res);
	return NULL;
    }

    size_t beg = 0;
    size_t end = 0;

    while (*(copy + end) != '\0')
    {
	if (*(copy + end) == '\'')
	{
	    res = middle_merge(res, copy, beg, end);
	    if (!res)
	        return NULL;

	    end++;
	    beg = end;

	    while (*(string + end) != '\0' && *(string + end) != '\'')
	    {
		end++;
	    }

	    if (*(string + end) == '\0')
	    {
		free(res);
		return NULL;
	    }

	    middle_merge(res, copy, beg, end);
	}
	end++;
    }

    char *tmp = strndup(string + beg, end - beg);
    res = merge(res, tmp);
    if (!res)
	    return NULL;

    return res;
}
