#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define dlltype void *
#define dllerror() strdup(dlerror())
void
module_close(void *dll)
{
    if (!dll)
        return;
    dlclose((dlltype) dll);
}


char *
module_load(const char *filename, const char *symbname,
            int (*shouldload)(void *symb, void *misc, char **err), void *misc,
            void **dll, void **symb)
{
    dlltype intdll = NULL;
    void *  intsym = NULL;
    char *  interr = NULL;

    if (dll)
        *dll = NULL;
    if (symb)
        *symb = NULL;


    intdll = dlopen(filename, RTLD_LAZY | RTLD_LOCAL);
    if (!intdll)
        goto fail;

    /* Get the module symbol */
    intsym = dlsym(intdll, symbname);
    if (!intsym)
        goto fail;

    /* Figure out whether or not to load this module */
    if (!shouldload(intsym, misc, &interr))
        goto fail;

    /* Re-open the module */
    module_close(intdll);
    intdll = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (!intdll)
        goto fail;

    /* Get the symbol again */
    intsym = dlsym(intdll, symbname);
    if (!intsym)
        goto fail;

    if (dll)
        *dll = intdll;
    if (symb)
        *symb = intsym;
    return NULL;

fail:
    if (!interr)
        interr = dllerror();
    module_close(intdll);
    return interr;
}
