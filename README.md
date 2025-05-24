# Asset Assembler Script (AAS)

AAS is a simple scripting language made with intention to generate all kinds of assets (like text files, images, etc.).
The syntax may look like assemble at first, but it's really different.
This language is stack-based (uses stack for most operations), but it also has variables (only global).

You can find the language reference in [docs](docs/Asset Assembler Script.md).

The library reference is work-in-progress.

The console module reference is work-in-progress.

The fio module reference is work-in-progress.

## Build

This project uses [bro](https://github.com/yetanotherfuryyman/bro).
You can use `build.ninja` or `Makefile` to build this project, but if you want to make some changes you should use bro.

To use bro:

``` sh
# You can use clang++ if you want.
# Bro will automaticly detect the used compiler and use it to rebuild itself when needed.
g++ bro.cpp -o bro
./bro
```

