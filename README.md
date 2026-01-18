# Hantek
Hantek 2D72 handheld oscillosope tool for linux

This is an unofficial implementation of a graphical tool to manage Hantek2D72 handheld oscilloscope.

The official tool is only for Windows, so I've developed this tool for personal use and I've decided to share it!

I've tested the tool only with my oscilloscope, so I don't guarantee it works with others.

The use of this tool is at your own risk, I don't grant its safety!

User confirmed Hantek 2C42 is supported too.

## Build
Change directory to the directory with the source code and execute:

    cmake .
    make
## udev rule to allow not root user to use the usb connection to the oscilloscope
Create a new file called for instance `99-hantek.rule` in folder `/etc/udev/rules.d` with the following content:

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", GROUP="plugdev", MODE="0666"

Restart or run as root:

    udevadm control --reload-rules && udevadm trigger

## run
Change directory to the directory with the source code and as normal user execute:

    ./Hantek

You can move the `Hantek' executable to the bin folder in your home folder (or any other folder with executables that is in the PATH variable):

    mv Hantek ~/bin/

Then you can execute it just by:

    Hantek
