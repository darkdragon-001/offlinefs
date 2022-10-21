# OFS extended attributes

The filesystem can be managed from the command line using extended attributes. There are two attributes available at the moment.

### ofs.offline

Indicated whether the directory or file is marked _offline_. If you set the attribute on a directory, the subtree, starting with this directory is copied to the offline cache. If you remove it, the offline cache for this tree is cleaned.

**Note:** You can remove the `ofs.offline` attribute only on the root of an offline tree.

### ofs.available

This attribute indicates whether there is a connection to the network
share, this file or directory is in.

## Using extended attributes

Make sure the package `attr` is installed.

### Get extended attribute

```sh
# getfattr -n <attribute name> <path>
```

### Set exteded attribute

```sh
# setfattr -n <attribute name> <path>
```

or

```sh
# setfattr -n <attribute name> -v <value> <path>
```

### Remove exteded attribute

```sh
# setfattr -x <attribute name> <path>
```

**Note:** The attributes `ofs.available` and `ofs.offline` are always available and contain one of the values _yes_ or _no_. Even if you remove `ofs.offline` it does not disappear but switches to _no_. Should we change this?

### List extended attributes

```sh
# getfattr -d <path>
```

**Note:** This does not list our OFS attributes, because attributes of a FUSE filesystem are only shown if they are in the namespace _user_ (i.e. name starts with _user._). This might be a bug in FUSE.
