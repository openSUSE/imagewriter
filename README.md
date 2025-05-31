imagewriter
===========

Utility for writing raw disk images &amp; hybrid isos to USB keys

Linux Installation
===========

In order to compile, run

** HAL **

    % qmake DEFINES=USEHAL imagewriter.pro

** udisks2 **

    % qmake DEFINES=USEUDISKS2 imagewriter.pro

Failing to specify the define will default to usedisk2.

Then run

    % make

Linux kiosk mode
===========

The imagewriter also has the capability to be used in "kiosks" that may have external harddrives.  To run in kiosk mode,
which both ignores large hard drives and marks unknown USB sticks as "SUSE Studio", 
run imagewriter with the -k option.

in order to start the imagewriter with sudo, do the following:

    % xhost +SI:localuser:root

to allow root access to the current screen and then start the program with:
 
    % sudo DISPLAY=:0 imagewriter

