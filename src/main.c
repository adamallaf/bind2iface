/*
 * bind2iface main.c
 * Copyright (c) 2024 Adam Allaf <adam.allaf@hotmail.com>
 *
 * This file is part of bind2iface, licensed under
 * the GNU General Public License, version 2.
 * For details, see the LICENSE file in the root of this project.
 */

#define _GNU_SOURCE

#include <ctype.h>
#include <err.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/version.h"

extern char **environ;
extern int optind;

char iface_name[16];
int verbose = 0;

void printUsageAndExit(int exit_status);

void printHelpAndExit(int exit_status);

int countNonOptArgs(char **argv, int *counter);

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    int non_opt = 0, cmd_ind = 1;
    if ( 2 > argc ) {
        printUsageAndExit(EXIT_FAILURE);
    } else if ( 2 <= argc ) {
        non_opt = countNonOptArgs(argv, &cmd_ind);

        int k            = 0;
        char **b2if_argv = (char **)malloc(sizeof(char *) * (cmd_ind + 1));
        for ( k = 0; k < cmd_ind; k++ ) {
            b2if_argv[k] = argv[k];
        }
        b2if_argv[k] = NULL;

        int arg;
        while ( (arg = getopt_long(k, b2if_argv, "hVv", long_options, 0)) != -1
        ) {
            switch ( arg ) {
            case 'h':
                free(b2if_argv);
                printHelpAndExit(EXIT_SUCCESS);
                break;
            case 'V':
                free(b2if_argv);
                fprintf(stderr, "bind2iface %s\n", VERSION);
                return EXIT_SUCCESS;
                break;
            case 'v':
                if ( verbose < 3 )
                    verbose += 1;
                break;
            default:
                free(b2if_argv);
                printUsageAndExit(EXIT_FAILURE);
            }
        }

        if ( 1 == non_opt ) {
            free(b2if_argv);
            fprintf(stderr, "%s: no command specified\n", *argv);
            printUsageAndExit(EXIT_FAILURE);
        } else if ( 0 == non_opt ) {
            free(b2if_argv);
            fprintf(stderr, "%s: no device specified\n", *argv);
            printUsageAndExit(EXIT_FAILURE);
        }

        strncpy(
            iface_name, b2if_argv[optind],
            MIN(sizeof(iface_name), strlen(b2if_argv[optind]) + 1)
        );
        free(b2if_argv);
    }

    const char *binary_name = argv[optind + 1];

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if ( 0 != chdir(cwd) ) {
        err(EXIT_FAILURE, "%s", cwd);
    }

    char lib_path[PATH_MAX];
    readlink("/proc/self/exe", lib_path, PATH_MAX - 1);

    sprintf(lib_path, "%s/%s", dirname(lib_path), "libbind2iface.so");
    char b2ifverbose[2];
    sprintf(b2ifverbose, "%d", verbose);

    setenv("LD_PRELOAD", lib_path, 1);
    setenv("B2IFACE_NAME", iface_name, 1);
    setenv("B2IFACE_VERBOSE", b2ifverbose, 1);

    return execvpe(binary_name, &argv[cmd_ind], environ);
}

void printUsageAndExit(int exit_status) {
    fprintf(
        stderr,
        "Usage:\n\tbind2iface [-v[v[v]]] DEVICE COMMAND [command_args]\n"
        "    or\n"
        "\tbind2iface [-h, --help]\n"
        "\n"
        "bind2iface %s\n",
        VERSION
    );
    exit(exit_status);
}

void printHelpAndExit(int exit_status) {
    fprintf(
        stderr, "Usage:\n\tbind2iface [options] DEVICE COMMAND [cmd_args]\n"
                "\n"
                "Options:\n"
                "\t-h, --help       print this help.\n"
                "\t-V, --version    print version and exit.\n"
                "\t-v[v[v]]         verbose, increase verbosity.\n"
                "\n"
                "\tDEVICE           network interface device, i.e.: eth0\n"
                "\tCOMMAND          command/program to execute.\n"
                "\n"
    );
    exit(exit_status);
}

/* counter should start from 1 because argv[0] must be skipped */
int countNonOptArgs(char **argv, int *counter) {
    int non_opt = 0;
    while ( argv[*counter] != NULL ) {
        non_opt += 0 || isalnum(argv[*counter][0]);
        if ( 2 == non_opt )
            break;
        (*counter)++;
    }
    return non_opt;
}
