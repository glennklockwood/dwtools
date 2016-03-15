#include <stdio.h>
#include <errno.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <datawarp.h>

#define _STAGE_IN   0
#define _STAGE_OUT  1

int main(int argc, char **argv)
{
    struct timespec t0, tf, dt;
    struct stat st;
    char *infile, *outfile;
    double dt_secs;
    int direction;
    int ret;

    if (argc < 4) {
        fprintf( stderr, "Syntax: %s <in|out> <source> <destination>\n", argv[0] );
        return -EINVAL;
    }

    if ( !strncasecmp( argv[1], "in", 2 ) ) {
        direction = _STAGE_IN;
    }
    else if ( !strncasecmp( argv[1], "out", 3 ) ) {
        direction = _STAGE_OUT;
    }
    else {
        fprintf( stderr, "Syntax: %s <in|out> <source> <destination>\n", argv[0] );
        return -EINVAL;
    }

    infile = argv[2];
    outfile = argv[3];

    /* for stage-in, make sure the file exists */
    if ( direction == _STAGE_IN ) {
        if ((ret = stat( infile, &st)) != 0 ) {
            fprintf( stderr, "Error: stat(%s) returned %d\n", infile, ret );
            return ret;
        }
        else if ( !S_ISREG( st.st_mode ) ) {
            fprintf( stderr, "Error: source file %s is not a normal file (directory stage-in not currently supported)\n", infile );
            return -ENOSYS;
        }
    }

    /*
     *
    int dw_stage_file_in( const char *dw_file_path, const char *pfs_file_path )
    int dw_stage_file_out( const char *dw_file_path, const char *pfs_file_path, enum dw_stage_type stage_type )
     *
     */
    if ( S_ISREG( st.st_mode ) ) {
        int ret_stage, ret_wait;
        printf( "infile is %s\noutfile is %s\n", infile, outfile );
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if ( (direction == _STAGE_IN && (ret_stage = dw_stage_file_in(outfile, infile)) == 0)
        ||   (direction == _STAGE_OUT && (ret_wait = dw_stage_file_out(infile, outfile, DW_STAGE_IMMEDIATE)) == 0) ) {
            ret_wait = dw_wait_file_stage(outfile);
        }
        else {
            fprintf( stderr, "Error: dw_stage_file_{in,out} returned %d\n", ret_stage );
            ret_wait = 0;
        }
        clock_gettime(CLOCK_MONOTONIC, &tf);
        if ( ret_wait != 0 ) {
            fprintf( stderr, "Error: dw_wait_file_stage returned %d\n", ret_wait );
        }
    }
    else if ( S_ISDIR( st.st_mode ) ) {
        fprintf(stderr, "Error: directory operations are not yet supported\n");
        return 2;
    }
    else {
        fprintf( stderr, "Error: %s is neither file nor directory\n", infile );
        return 1;
    }

    /* get size of file as it exists on the pfs */
    if ( (ret = stat( infile, &st)) != 0) {
        fprintf( stderr, "Error: stat(%s) returned %d\n", infile, ret );
        return ret;
    }

    /* calculate the time delta */
    if ( (tf.tv_nsec - t0.tv_nsec) < 0 ) {
        dt.tv_sec = tf.tv_sec - t0.tv_sec - 1;
        dt.tv_nsec = 1000000000 + tf.tv_nsec - t0.tv_nsec;
    } 
    else {
        dt.tv_sec = tf.tv_sec - t0.tv_sec;
        dt.tv_nsec = tf.tv_nsec - t0.tv_nsec;
    }
    dt_secs = dt.tv_sec + dt.tv_nsec / 1e9;

    printf( "%ld bytes in %f sec - %f MiB/sec\n",
        st.st_size,
        dt_secs,
        (double)st.st_size / dt_secs / 1024.0 / 1024.0 );

    return 0;
}
