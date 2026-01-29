#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"
#include "parser/ast.h"

#define functions_hashmap_SIZE 16

static struct functions_hashmap *functions_hashmap = NULL;

static struct functions_hashmap *functions_hashmap_init(size_t size)
{
    struct functions_hashmap *h = malloc(sizeof(struct functions_hashmap));
    if (!h)
        return NULL;

    h->data = calloc(size, sizeof(struct ast_funcdec *));
    if (!h->data)
    {
        free(h);
        return NULL;
    }

    h->size = size;

    return h;
}

int init_functions_hashmap(void)
{
    if (functions_hashmap != NULL)
    {
        warnx("init_functions: Try to initialize an already initialised module "
              "!!!");
        return 1;
    }

    functions_hashmap = functions_hashmap_init(functions_hashmap_SIZE);

    return 0;
}

void reset_functions_hashmap(void)
{
    functions_hashmap_free(functions_hashmap);
    functions_hashmap = NULL;
}

bool functions_hashmap_insert(char *name, struct ast *ast)
{
    if (functions_hashmap == NULL || functions_hashmap->size == 0
        || key == NULL)
        return false;

    size_t place = hash(key) % functions_hashmap->size;
    struct function_pair_list *list =
        (struct function_pair_list *)functions_hashmap->data[place];
    while (list != NULL)
    {
        if (strcmp(list->name, name) == 0)
        {
            free_ast(list->function);
            list->function = ast;

            return true;
        }

        list = list->next;
    }

    struct functions_pair_list *v = malloc(sizeof(struct functions_pair_list));
    if (!v)
        return false;

    v->name = strdup(name);
    // After a function definition, the corresponding AST is removed of the
    // parent AST
    v->function = ast;
    v->next = functions_hashmap->data[place];

    functions_hashmap->data[place] = v;

    return true;
}

struct ast *functions_hashmap_get(const char *name)
{
    if (functions_hashmap == NULL || functions_hashmap->size == 0
        || name == NULL)
        return NULL;

    size_t place = hash(name) % functions_hashmap->size;
    struct functions_pair_list *list = functions_hashmap->data[place];
    while (list != NULL)
    {
        if (strcmp(list->name, name) == 0)
            return list->function;

        list = list->next;
    }

    return NULL;
}

static void functions_pair_list_free(struct functions_pair_list *list)
{
    if (!list)
        return;

    free(list->name);
    free_ast(list->function);

    functions_pair_list_free(list->next);
    free(list);
}

bool functions_hashmap_remove(const char *key)
{
    if (functions_hashmap == NULL || functions_hashmap->size == 0
        || name == NULL)
        return false;

    size_t place = hash(name) % functions_hashmap->size;
    struct functions_pair_list *list = functions_hashmap->data[place];

    if (list == NULL)
        return true;

    if (strcmp(list->name, name) == 0)
    {
        functions_hashmap->data[place] = list->next;
        free(list);

        return true;
    }

    while (list->next != NULL)
    {
        if (strcmp(list->next->name, name) == 0)
        {
            struct functions_pair_list *t = p->next;
            p->next = t->next;

            t->next = NULL;
            functions_pair_list_free(t);

            return true;
        }

        p = p->next;
    }

    return false;
}

void functions_hashmap_free(struct functions_hashmap *)
{
    if (functions_hashmap == NULL)
        return;

    for (size_t i = 0; i < functions_hashmap->size; i++)
        functions_pair_list_free(functions_hashmap->data[i]);

    free(functions_hashmap->data);
    free(functions_hashmap);
}
