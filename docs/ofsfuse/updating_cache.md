# Updating single files in the cache

Offline trees get updated on demand. This is done by the ofs daemon.

If a file in an offline tree is accessed, OFS updates this file immediately if it is out of date. This is done by OFS callbacks, since the file has to be current prior to any access.

### Advantages

* Frequently used files are always current when connection is lost

### Disadvantages

* Access on a file is blocked until the complete file has copied to the cache. This can result in timeout or inconvenience to the user
* Since each open triggers a `stat` on both the remote and the cached file, it is a bit slower.

### Possible Solutions

* Copying the file to the cache could be executed asynchronous using the OFS daemon
