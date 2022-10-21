# Developing OFS with Eclipse IDE

The official IDE for developing OFS is Eclipse. If you don't like Eclipse you are free to use vim, emacs or any other editor or IDE you like.

This page will guide you in setting up your development environment on Eclipse Helios (3.6).

## Prerequisite Plug-Ins

* C Development Tools (CDT)
* Autotools PlugIn
* Subversion Client (Subversive or Subclipse)

The following two sections will guide you through the installation of Eclipse and the required Plug-Ins. If you are familiar with Eclipse, skip them.

### Installing Eclipse

First of all make sure you are not in hurry. Eclipse is not one of the fastest IDEs, but one of the must usable. ;-)

- Install a JAVA runtime environment Version 6
- Download _Eclipse IDE for C/C++ Linux Developers_ from [here](http://www.eclipse.org/downloads/) and extract it. (Note: This guide is for Eclipse Helios. If you use another version of Eclipse, the following steps may vary.md)
- Unpack the file, start eclipse and choose a workspace

This package already contains CDT and Autotools PlugIn.

### Installing Subversive

Subversive is - beside Subclipse - one of the two most popular Plug-Ins to add subversion support to Eclipse.

- Choose “Help → Install new Software” and select _"All available sites"_
- If you are using a eclipse version prior to indigo enter <http://download.eclipse.org/technology/subversive/0.7/update-site/> as update site
- You will now be provided with a list of Plug-Ins. Select _“Subversive SVN Team Provider”_ which is located below _"Collaboration"_ or _“Subversive SVN Team Provider Plugin"_, depending on your Eclipse version
- Finish the steps of the Wizard and restart Eclipse
- After restarting Eclipse, wait for the _“Subversive Connector Discovery”_ window to appear and select a SVN connector.
  If you go for JavaHL, you have to install the libsvn-java package (ubuntu name) in advance. SVNKit does not need additional software.

## Checking out OFS from SVN

If you are familiar with your SVN Plug-In you can skip this section. Otherwise keep on reading.

- Call: _File → Import → SVN → Project from SVN_ and click _Next_
- Enter in the URL <http://offlinefs.svn.sourceforge.net/svnroot/offlinefs/ofs> and add your credentials if you have write access
- in the next page click _Browse_ and select the trunk or whatever branch you want to develop on.
- Finish the Wizard
- In the upcoming window choose _Check out as a project with the name specified_

## Building OFS

Before building OFS you have to run the autotools. If this is not done
automatically by eclipse:

- Select the newly created project in the Project Explorer
- Call _Project → Invoke Autotools → Invoke autoreconf_ from the menu (don't enter any parameters).

When building the project, you can choose from two build configurations _Build_ and _Debug_. This second one will build with debug symbols and without optimization. The binaries for the launch configurations will reside in the _build-Release_ and _build-Debug_ subdirectories repectively.

- Click the hammer icon in the toolbar to build the project. Use the little arrow at the right side of this icon to choose the build configuration.

## Running OFS

OFS comes with two predefined launch configurations.

- Call _Run → Run Configurations..._
- Select _ofs debug_ below _C/C++ Application_
- Switch to the _Arguments_ tab and customize it for your needs. See [Using the command line of the OFS executable](../ofscommandline/index.md) for more information.
- Click _Run_ (Or the Play icon on the toolbar) - OFS will start now

Note that you have to use "run" not "debug" to start OFS.

Please **never** commit your custom run configurations to SVN.

Since the OFS executable immediately returns, the debugger must subsequently be attached to the running OFS daemon.

- Run the _ofs Attach Debugger_ from the debugger (little arrow at the right side of the bug icon)
- Select the ofs process and klick OK
