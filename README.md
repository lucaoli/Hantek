# Hantek
Hantek 2D72 handheld oscillosope tool for linux

This is an unofficial implementation of a graphical tool to manage Hantek2D72 handheld oscilloscope.

The official tool is only for Windows, so I've developed this tool for personal use and I've decided to share it!

I've tested the tool only with my oscilloscope, so I don't guarantee it works with others.

The use of this tool is at youw own risk, I don't grant its safety!

# Build Instructions

To build this program so dependencies will need to be met

On Fedora 36+ execute 
``` sudo dnf -y install gtk3-devel libusb-devel cmake```

``` git clone https://github.com/lucaoli/Hantek.git && cd Hantek```

``` cmake CMakeLists.txt && make && sudo cp Hantek /usr/local/bin/hantek```

# Running this software

This program requires permissions to execute. 

On Fedora 36+ execute

sudo /usr/local/bin/hantek
