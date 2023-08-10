
int
module_symbol_is_present(const char *modname, const char *symbname);


int
module_get_filename_for_symbol(void *addr, char **filename);

void
module_close(void *dll);

char *
module_load(const char *filename, const char *symbname,
            int (*shouldload)(void *symb, void *misc, char **err), void *misc,
            void **dll, void **symb);
