/*
 * dw_get_stripe_configuration: a wrapper around libdatawarp's
 * dw_get_stripe_configuration API call which populates a few integer pointers
 * with basic information on a file descriptor's stripe width, size, and offset.
 *
 * Run from within a job script (e.g., on a compute node) and specify the path
 * to a file on DataWarp as the sole command-line argument, e.g.,
 *
 *     touch $DW_JOB_STRIPED/hello
 *     ./dw_get_accounting_data $DW_JOB_STRIPED/hello
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <datawarp.h>

int main(int argc, char **argv) {
    int fd;
    int stripe_size = -1,
        stripe_width = -1,
        starting_index = -1;
    int err;

    if ( argc < 2 ) {
        fprintf(stderr, "Syntax: %s <path to dw mount>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0 ) {
        perror(NULL);
        return 1;
    }

    err = dw_get_stripe_configuration(fd,
                                      &stripe_size,
                                      &stripe_width,
                                      &starting_index);

    printf("%s: stripe_size=%d, stripe_width=%d, starting_index=%d\n",
        argv[1],
        stripe_size,
        stripe_width,
        starting_index);
    return err;
}
