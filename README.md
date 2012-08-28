imagewriter
===========

Utility for writing raw disk images &amp; hybrid isos to USB keys

Linux Installation
===========

In order to compile, run

    % qmake 

to create the make file and then run

    % make

qmake should automatically detect your platform.

Linux kiosk mode
===========

The imagewriter also has the capability to be used in "kiosks" that may have external harddrives.  To run in kiosk mode,
which both ignores large hard drives and marks unknown USB sticks as "SUSE Studio", 
run imagewriter with the -k option.

in order to start the imagewriter with sudo, do the following:

    % xhost +SI:localuser:root

to allow root access to the current screen and then start the program with:
 
    % sudo DISPLAY=:0 imagewriter

