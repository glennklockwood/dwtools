#include <stdio.h>
#include <datawarp.h>

int main(int argc, char **argv) {
    char *json;
    int err;

    if ( argc < 2 ) {
        fprintf(stderr, "Syntax: %s <path to dw mount>\n", argv[0]);
        return 1;
    }

    json = dw_get_accounting_data_json(argv[1], &err);

    printf("%s\n", json);
    return err;
}
