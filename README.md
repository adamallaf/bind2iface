# bind2iface
[![CMake on a single platform](https://github.com/adamallaf/bind2iface/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/adamallaf/bind2iface/actions/workflows/cmake-single-platform.yml)

Binds socket calls to a specified interface.

## Compile & Install

The default installation prefix is `${HOME}/bin`, override it with `-DCMAKE_INSTALL_PREFIX=/custom/prefix/dir` or `--prefix custom/prefix/path`.

```bash
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/ --config Release
cmake --install build/ --config Release
```

## Usage

```bash
Usage:
        bind2iface [options] DEVICE COMMAND [cmd_args]

Options:
        -h, --help       print this help.
        -V, --version    print version and exit.
        -v[v[v]]         verbose, increase verbosity.

        DEVICE           network interface device, i.e.: eth0
        COMMAND          command/program to execute.
```

For instance if firefox-esr is to be launched without internet access it can use loopback interaface, by binding socket calls to `lo`:

```bash
bind2iface -vvv lo firefox-esr --private-window 127.0.0.1:8080
```
