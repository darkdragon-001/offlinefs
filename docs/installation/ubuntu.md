# Compiling OFS on Ubuntu 7.10

## Required Packages

* `libfuse-dev`
* `pkg-config`
* `libtool`
* `automake`
* `autoconf`
* `g++`
* `libattr1-dev`
* `attr`
* `libconfuse-dev`
* `libdbus-1-dev`

For creating the doxygen documentation you need

* `doxygen`
* `graphviz `
* `texlive-extra-utils`

## Getting the source

Checkout the source from subversion 
```sh
svn co http://offlinefs.svn.sourceforge.net/svnroot/offlinefs/ofs/trunk ofsproject
```

## Compiling

```sh
# aclocal
# libtoolize
# autoheader
# automake
# autoconf
# ./configure
# make
# sudo make install
```

## Using

You can now mount a filesystem through OFS with the command

```sh
# mount -t ofs fstype://server/share /some/mountpoint
```

Please note that you have to be root to so this - or prepend the command with `sudo`.

More Information can be found at [Mount helper](../mounthelper/index.md).
