# Compiling OFS on Mac OS X

## Mac Ports

- Install the following packages form Mac Ports
  * `automake`
  * `libtool`
  * `autoconf`
  * `fuse4X`
  * `gcc45`
  * `dbus-devel`
  * `libconfuse`
- `autoreconf -i`
- `./configure`
- `make`
- `make install`

### Issues

- On Mac OS X 10.7.3 using XCode 4.3.1 and macports 2.0.4 installing `libconfuse` was reported to fail with _"Error: Dependency 'libiconv' not found."_
  * Try 
    `sudo port -v selfupdate`
  * Refer to [macports ticket 33650](https://trac.macports.org/ticket/33650.md)
  * Thanks Olivier for reporting this!

## Fink

tbd.
