#include <arpa/inet.h>
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <net/if.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>

#include "utils.h"

#ifndef LIBC_SO
#define LIBC_SO "libc.so.6"
#endif

#define B2IF_NAME "B2IFACE_NAME"

typedef int (*volatile socket_t)(int domain, int type, int protocol);
typedef int (*volatile connect_t)(
    int fd, const struct sockaddr *addr, socklen_t len
);
typedef int (*volatile syscall_t)(long int number, ...);

static socket_t libc_socket;
static connect_t libc_connect;
static syscall_t libc_syscall;

static char *iface_name = 0;

static void init_libc_symbols() {
    dlerror(); /* clear old errors */
    void *libc_ptr = dlopen(LIBC_SO, RTLD_LAZY);
    if ( !libc_ptr ) {
        errx(EXIT_FAILURE, "[-] Could not load libc: %s", LIBC_SO);
    }

    dlerror(); /* clear old errors */
    LOAD_SYMBOL(socket, libc_ptr)
    LOAD_SYMBOL(connect, libc_ptr)
    LOAD_SYMBOL(syscall, libc_ptr)

#ifdef DEBUG
    fprintf(stdout, "[+] loaded libc symbols\n");
#endif

    dlclose(libc_ptr);
}

int socket(int domain, int type, int protocol) {
    struct ifreq ifr;

#ifdef DEBUG
    fprintf(stderr, "socket %d %d %d\n", domain, type, protocol);
#endif

    int sockfd = libc_socket(domain, type, protocol);
    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(
        ifr.ifr_name, iface_name,
        MIN(strlen(iface_name) + 1, sizeof(ifr.ifr_name))
    );

    if ( setsockopt(
             sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr,
             sizeof(struct ifreq)
         ) < 0 ) {
        errx(EXIT_FAILURE, "%s: \"%s\"", strerror(errno), ifr.ifr_name);
    }

    return sockfd;
}

int connect(int fd, const struct sockaddr *addr, socklen_t len) {
    const struct sockaddr_in *tmp = (const struct sockaddr_in *)addr;

    char *buf = (char *)malloc(32);

    inet_ntop(AF_INET, &tmp->sin_addr, buf, 32);
    fprintf(stderr, "connect %d %s:%d\n", fd, buf, ntohs(tmp->sin_port));

    free(buf);
    return libc_connect(fd, addr, len);
}

int syscall(long int number, ...) {
    va_list args;
    fprintf(stderr, "syscall %ld \n", number);
    va_start(args, number);
    int result = libc_syscall(number, args);
    va_end(args);
    return result;
}

void __attribute__((constructor)) lib_init(void) {
    init_libc_symbols();

    iface_name = getenv(B2IF_NAME);
    if ( iface_name == NULL )
        errx(
            EXIT_FAILURE, "[-] No interface provided, please set %s=<iface>",
            B2IF_NAME
        );
    fprintf(stdout, "[*] using network interface: %s\n", iface_name);
}

void __attribute__((destructor)) lib_deinit(void) { ; }
