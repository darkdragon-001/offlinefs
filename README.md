# Offline File System (OFS)
 
## State
 
Offline File System is still in alpha state and not very stable.
 
Fork of http://offlinefs.sourceforge.net/
 
## Building
 
### Required packages

    sudo apt-get install libfuse-dev pkg-config libtool automake autoconf g++ libattr1-dev attr libconfuse-dev libdbus-1-dev

### Optional packages for documentation

    sudo apt-get install doxygen graphviz texlive-extra-utils

### Compilation

    libtoolize
    aclocal
    autoheader
    automake --add-missing
    autoconf
    ./configure
    make
    sudo make install

## Usage

### Mount

    sudo mount -t ofs fstype://server/share /some/mountpoint

### Make folders available offline

    setfattr -n ofs.offline /path/to/directory
 
### Go offline

    setfattr -x ofs.available /mnt

### Go online

    setfattr -n ofs.available /mnt

### Find conflicts

    getfattr -n ofs.conflict /mnt
    getfattr -n ofs.conflict /mnt/*

### Resolve Conflicts

    setfattr -n ofs.conflict -v=local /path/to/conflicted/file
    setfattr -n ofs.conflict -v=remote /path/to/conflicted/file
