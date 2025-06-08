imagewriter
===========

Utility for writing raw disk images &amp; hybrid isos to USB keys

Linux Installation
===========

In order to compile, run

** HAL **

    % qmake DEFINES=USEHAL imagewriter.pro

or

    % cmake -DUSEHAL=ON ..

** udisks1 **
    
    % qmake DEFINES=USEUDISKS imagewriter.pro

or

    % cmake -DUSEUDISKS=ON ..

** udisks2 **

    % qmake DEFINES=USEUDISKS2 imagewriter.pro

or

    % cmake -DUSEUDISKS2=ON ..

Failing to specify the define will likely not work out.

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

