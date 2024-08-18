#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

char iface_name[16];
int verbose = 0;

int main(int argc, char *argv[]) {
    if ( 2 > argc ) {
        errx(EXIT_FAILURE, "no interface specified.");
        /* print usage here */
    }
    strncpy(iface_name, argv[1], MIN(sizeof(iface_name), strlen(argv[1]) + 1));

    if ( 3 <= argc && 0 == strncmp(argv[2], "-v", 2) ) {
        char *tmp = argv[2];
        int v;
        for ( v = 0; tmp[v] && v < 3; tmp[v] == 'v' ? v++ : *tmp++ )
            ;
        verbose = v;
    }

    int exec_args_offset = 2 + (verbose && 1);
    if ( exec_args_offset > argc ) {
        errx(EXIT_FAILURE, "too few arguments");
        /* print usage here */
    }
    const char *binary_name = argv[exec_args_offset];

    char *path_env = getenv("PATH");
    if ( NULL == path_env ) {
        errx(EXIT_FAILURE, "PATH environment variable not found");
    }

    char *path_dup = strdup(path_env);
    if ( NULL == path_dup ) {
        errx(EXIT_FAILURE, "could not duplicate PATH environment variable");
    }

    int path_not_found = 1;
    char binary_path[PATH_MAX];
    char *dir = strtok(path_dup, ":");
    while ( NULL != dir ) {
        snprintf(binary_path, sizeof(binary_path), "%s/%s", dir, binary_name);

        if ( 0 == access(binary_path, X_OK) ) {
            path_not_found = 0;
            free(path_dup);
            break;
        }

        dir = strtok(NULL, ":");
    }

    if ( path_not_found ) {
        errx(EXIT_FAILURE, "%s: command not found", binary_name);
    }

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if ( 0 != chdir(cwd) ) {
        err(EXIT_FAILURE, "%s", cwd);
    }

    char b2ifname[32];
    char b2ifverbose[20];
    sprintf(b2ifname, "B2IFACE_NAME=%s", iface_name);
    sprintf(b2ifverbose, "B2IFACE_VERBOSE=%d", verbose);

    char *const envs[] = {
        "LD_PRELOAD=./libbind2iface.so", b2ifname, b2ifverbose, NULL
    };

    return execve(binary_path, &argv[exec_args_offset], envs);
}
