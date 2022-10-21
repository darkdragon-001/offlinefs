# Purpose of the classes `ofs_fuse` and `OFSFile`

## FUSE C++ binding

The FUSE framework is originally designed for use in C programms. There are bindings available for several other programming languages like Java, C# and Perl. Since OFS is written in C++, we chose the fusexx C++ binding. This class simply wraps the C callback functions into static class methods. The callbacks have to be implemented in a subclass of this wrapper class.

More information and download at: <http://portal.itauth.com/2007/07/07/c-fuse-binding>

## The static class `ofs_fuse`

This class is a subclass of the fusexx wrapper class. All callbacks are implemented here. The main purpose of all these callbacks is to acquire an instance of `OFSFile` and call the appropriate function on it.

The callback methods work in one of two ways

1. The function operates on a file which has _not_ been opened before
   * Use the Filestatusmanager to fetch a `File` object with all information on the file
   * Pass the `File` object to the constructor of a new `OFSFuse` object.
   * call the desired method on the `OFSFuse` object
   * On `open()` calls, store a pointer to the `OFSFuse` object in the filehandle field of the `fuse_file_info` structure.
2. The function operates on a file which has been opened before: Since FUSE provides the same `fuse_file_info` structure in all calls from open until close, we can simply take the `OFSFile` object from the filehandle field.


## The class `OFSFile`

Should probably renamed to `FUSEFile`.

This class is part of the FUSE callbacks. One instance is assigned to exactly one (open) file or directory. Every time a FUSE callback is executed on a particular file, an `OFSFile` instance is acquired.
