/*
 * bind2iface utils.h
 * Copyright (c) 2024 Adam Allaf <adam.allaf@hotmail.com>
 *
 * This file is part of bind2iface, licensed under
 * the GNU General Public License, version 2.
 * For details, see the LICENSE file in the root of this project.
 */

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
