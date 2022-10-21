# Monitor thread

The OFS monitor thread is a thread, started in the init function of [FUSE callbacks](../ofsfuse/index.md). This means, each OFS managed mount point has its own monitor thread.

This thread is responisble for:

* monitoring the network connection
* synchronizing remote share and local cache on demand
