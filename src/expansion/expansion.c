#include "expansion/expansion.h"

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

char *expand_string(char *string)
{

}

char *expand_single_quote(char *string)
{

}

char *expand_escape(char *string)
{

}
