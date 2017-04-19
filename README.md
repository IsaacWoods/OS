# OS
A small x86 OS and userspace. It doesn't aim to be POSIX compliant, and will support most features of LibC.

# Building
This should probably by done on a *NIX with a recent GCC cross-compiler built for the `i686-elf` target.
If `grub-mkrescue` complains, try running `sudo apt install xorriso grub-pi-bin` (or equivalent).

* Build with `make`
* Clean environment with `make clean`
* Emulate with `make qemu`

# Warning
**Building the ISO yourself is fine**, but distributing the resultant `.iso` infringes the GPL, because
`grub-mkrescue` includes code from GRUB. You either can't distribute the ISO or have to also distribute the entire
GRUB source.
