# Getting started with OFS

## Installation

* First of all, get the sources and compile OFS.
* Create the directory `/var/ofs`

## First try

**Note:** By now only root can use OFS, because it uses the `mount` command internally.

Now you should be able to mount a remote share.

Start by simply mirroring a directory. For example mount the `/usr` directory to `/mnt`. This can be done using the command: 

```sh
# mount -t ofs file://usr /mnt
```

You will see the word _Starting_ at the prompt again. If you execute the command

```sh
# mount
```

you should see a line similar to the following, indicating that `/mnt` is now an ofs managed mountpoint

```
ofs on /mnt type fuse.ofs (rw,nosuid,nodev,allow_other)
```

Change to `/mnt` now and see your files.

Now you can try to mount a samba share (we haven't tested OFS with any other filesystem by now).

```sh
# mount -t ofs smbfs://some.server/some/share /some/mountpoint
```

On some systems, you have to use `smb://` instead of `smbfs://`.

**IMPORTANT NOTICE:**

* Use the command in a way you do not have to type any username/password or the like interactively. Otherwise the connections to the remote FS cannot be re-established by OFS when going online
* The mount options, this means also the password, will be stored in clear text on the local harddisk!

Of course this will change in the future.

Example for mounting a SMB share with username and passwort set in the mount command:

```sh
# mount -t ofs smbfs://some.server/some/share /some/mountpoint -o username=myuser,password=mypass
```

## Making folders offline available

To mark a folder (and all its sub-files and folders recursively) available offline, extended attributes are used. Issue the following command:

```sh
# setfattr -n ofs.offline /path/to/directory
```

See [Extended attributes](attributes.md) for more information about the command line interface.

On Mac OS X use:
```sh
$ xattr -w ofs.offline /path/to/directory
```

## Going offline

To switch to offline mode issue the following command on the root of your mounted filesystem

```sh
# setfattr -x ofs.available /mnt
```

You are now working in disconnected mode. This means, the remote filesystem has been unmounted and you can safely remove you network connection.

When browsing your share you can see only the directories you made offline before. You can edit, add and remove files and directories the same way as if you were online.

On Mac OS X use:

```sh
$ xattr -d ofs.available /mnt
```

## Going online

Make sure the network connection to your server is restored

Enter the following command on the root of your ofs mountpoint to switch back online

```sh
# setfattr -n ofs.available /mnt
```

Now your remote filesystem will be mounted again and the changes you made during offline mode will be reintegrated to the server.

You can now see your complete share again.

## Finding conflicts

If you change a file on the client and someone edits the same file on the server while you work in disconnected mode this file remains in _conflicted_ state and does not get reintegrated.

To find conflicted files, the attribute `ofs.conflict` is used. It tells you if

* the given file is in conflicted state
* some file(s) is/are conflicted somewhere in the tree below the given directory

```sh
# getfattr -n ofs.conflict /mnt
```

shows `yes` if there is a conflicted file below the `/mnt` directory.

```sh
# getfattr -n ofs.conflict /mnt/*
```

List all files and directories below the `/mnt` directory and indicates for each one whether it is conflicted or not. In such a way you can go step-by-step from the root to the conflicted file.

## Resolving conflicts

When you have found a conflicted file, you can decide if the local should overwrite the file in the server or the other way.

The following command accepts the _local_ file as current and overwrites the file on the server:

```sh
# setfattr -n ofs.conflict -v=local /path/to/conflicted/file
```

To make the _remote_ file current and overwrite the local file execute:

```sh
# setfattr -n ofs.conflict -v=remote /path/to/conflicted/file
```


