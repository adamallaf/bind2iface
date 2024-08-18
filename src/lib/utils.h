#ifndef __BIND2IFACE_UTILS_H__
#define __BIND2IFACE_UTILS_H__

#define DEBUG

#define _STR(x) #x
#define STR(x) _STR(x)

#define LOAD_SYMBOL(name, libptr)                                              \
    libc_##name = (name##_t)dlsym(libptr, STR(name));                          \
    if ( dlerror() != NULL ) {                                                 \
        errx(EXIT_FAILURE, "[-] Could not load function %s", STR(name));       \
    }

#endif
