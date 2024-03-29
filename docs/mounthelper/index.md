# Mount helper

## What is a mount helper?

The mount helper is a program, located in `/sbin`. It is called `mount.xxx` where xxx is the name of the filesystem. If the mount command is called using `mount -t xxx` it searches for a executable, called `/sbin/mount.xxx` and runs it with the parameters, given to the mount command.

## OFS mount helper

The OFS mount helper mounts the remote share and calls the ofs with the appropriate parameters.

### Installing the mounthelper

* Copy the `ofsmount` binary from the `mounthelper` directory to `/sbin/mount.ofs`
* Make sure the ofs binary is called `ofs` and is located somewhere in `PATH`

### Calling the mounthelper

The mounthelper is automatically called by the mount command, when the filesystem ofs is desired. As Device, you give a URL, which contains the remote filesystem as protocol and the share as path.

```sh
# mount -t ofs filesystem://path/to/share /some/local/mountpoint
```

In order to mount a samba filesystem through OFS, you use (only as root of course) the command:

```sh
# mount -t ofs smb://fileserver/share /some/local/mountpoint
```
