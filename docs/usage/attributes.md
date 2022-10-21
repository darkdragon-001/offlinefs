# Extended attributes

Interaction with OFS is done using extended attributes. So before you can start, you have to install the tools for using extended attributes, which ship with almost any linux distribution.

OFS supports the following attributes
| name            | action     | description |
| --------------- | ---------- | --- |
| `ofs.offline`   | **set**    | make directory and all its subdirectories available offline |
| `ofs.available` | **remove** | switch to offline mode |
| `ofs.available` | **set**    | switch to online mode|
| `ofs.available` | **read**   | see if the share is connected |
| `ofs.conflict`  | **read**   | see if the _given file_ or any file in the tree below the _given directory_ is in conflicted state |
| `ofs.conflict`  | **write**  | choose which file is current. The other one will be overwritten. Possible values: `local`, `remote` |

To make the a path available offline, use the following command

```sh
# setfattr -n ofs.offline /path/to/directory
```

You can see if a file is currently available by using the command

```sh
# getfattr -n ofs.available /path/to/directory/or/file
```
