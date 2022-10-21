# Using the command line of the OFS executable

The OFS executable `ofs` is the main component of OFS. It mounts the filesystem to the specified path and starts the ofs daemon. Normally only the [Mount helper](../mounthelper/index.md) calls the OFS binary directly.

OFS is called as follows:

```sh
# ofs <mountpoint> <shareurl> [<options>]
```

- `mountpoint:` The mountpoint where the user wants to have the filesystem mounted to.
- `shareurl:` The URL of the share as passed to the mounthelper. **Note:** The share is NOT mounted by this program. This is done via the mount-helper. The URL is necessary for example for calculating the share ID.

Options can be any of the following:
| short | long        | description |
| ----- | ----------- | ----------- |
| `-r`  | `--remote`  | Define the path, the remote share is mounted to. By default the remote mountpoint directory is fetched from the configuration file (default is `/var/ofs/remote`) and the share ID is appended as subdirectory |
| `-b`  | `--backing` | Define the path, the cache for the share is located. By default the backing directory is fetched from the configuration file (default is `/var/ofs/backing`) and the share ID is appended as subdirectory |
| `-l`  | `--listen`  | Network device, the ofs listens for plug/unplug events. Default: Fetched from configuration file |
| `-i`  | `--shareid` | Specify a different unique share ID. By default libofshash is used to calculate the hash from the share URL |
| `-h`  | `--help`    | display a help screen and exit |

**Note:** All Directories used by OFS (mountpoint, remote mountpoint, backing path) have to exist before OFS is started!
