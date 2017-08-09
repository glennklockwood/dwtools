/*
 * dw_get_accounting_data: a wrapper around libdatawarp's
 * dw_get_accounting_data_json API call which returns a number of per-fragment
 * and realm-wide server-side metrics as a json-encoded string to stdout.
 *
 * Run from within a job script (e.g., on a compute node) and specify the path
 * to a DataWarp mount point as the sole command-line argument, e.g.,
 *
 *     ./dw_get_accounting_data $DW_JOB_STRIPED
 *
 * or 
 *
 *     ./dw_get_accounting_data $DW_JOB_PRIVATE
 */
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
