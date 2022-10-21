# Welcome to OFS

The OFS (Offline FileSystem or Ohm FileSystem) is a offline-layer which
is (or should be) able to extend every filesystem with offline
capabilities. This means, that the user is able to work on his network
documents, even when he is disconnected from the server. When connecten
is restored, the filesystem automatically synchronizes the files.

This project is developed at the [Georg Simon Ohm University of Applied
Sciences Nuremberg (Germany)](http://www.ohm-university.eu)
and is mentored by [Prof. Dr. Peter
Trommler](http://www.informatik.fh-nuernberg.de/top.asp?url=http://www.informatik.fh-nuernberg.de/Professors/Trommler/HomeIN.asp).
It was inspired by many different projects like Coda, InterMezzo and
Windows Offlinefiles and was kicked off by the [Diploma Theses of Tobias
Jähnel](http://da.jonmedia.net).

The first prototype has been implemented by Frank Gsellmann, [Tobias
Jähnel](http://www.jonmedia.net) and [Carsten
Kolassa](http://www.kolassa.net) as a student research
project.

Sourceforge Project page: <http://sourceforge.net/projects/offlinefs>

## Mailing List and Bug tracker

You can subscribe to the OFS mailing list at
<https://lists.sourceforge.net/lists/listinfo/offlinefs-devel> Feel free
to post any questions on the offline filesystem.

If you found a bug, please file a bug report into our Bugtracking system
at <http://apps.sourceforge.net/mantisbt/offlinefs>

## Linux magazine articles online

In January 2009 an article about OFS, written by the original team was
issued in the german _Linux Magazin_. It has been translated and came
up in the english _Linux Pro Magazine_ in February as well.

Now both articles available for reading online.

English: <http://www.linuxpromagazine.com/Issues/2009/99/CONNECTIONS/>

German: <http://www.linux-magazin.de/Heft-Abo/Ausgaben/2009/01/Verbindungssuche>

The article talks about the idea of OFS, our concepts and ideas. It's worth reading.

## Current progress

The first working prototype has been finished. It is possible to make files from SMB filesystems offline available, reintegrate and there is a rudimentary possibility to detect and solve conflicts manually.

At the moment there is an IT-Project at Georg-Simon-Ohm University running.

## Getting started

* [Installation](installation/index.md)
* [Getting started with OFS](usage/getting_started.md)

## OFS Project

* [Extended attributes](usage/attributes.md)
* [FUSE callbacks](ofsfuse/index.md)
* [Monitor thread](ofsdaemon/index.md)
* [Remote filesystem mounter](mounter/index.md)
* [Mount helper](mounthelper/index.md)
* [Using the command line of the OFS executable](ofscommandline/index.md)
