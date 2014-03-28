Netctl GUI
==========

Information
-----------
Graphical interface for netctl. Now it may work with profiles and may create new profiles. Also it may create a connection to WiFi. Moreover, it provides a widget and DataEngine for KDE.

Configuration
-------------
It is recommended to use graphical interface for widget configuration. DataEngine settings are stored in /usr/share/config/netctl.conf:
* `CMD` - netctl command. Default value is `/usr/bin/netctl`.
* `IPCMD` - ip command. `/usr/bin/ip` by default.
* `NETDIR` - a directory, which contains subdirectories with names of network interfaces. Default is `/sys/class/net/`.
* `EXTIPCMD` - a command, which returns external IP address. Default value is `wget -qO- http://ifconfig.me/ip`.
* `EXTIP` - `true` will enable external IP definition. Default is `false`.
All settings of GUI are stored in `$HOME/.config/netctl-gui.conf`. It is highly recommended to edit it from graphical interface.

Instruction
===========

Dependencies
------------
* netctl
* qt4

Optional dependencies
---------------------
* kdebase-workspace (widget)
* sudo (sudo support)
* wpa_supplicant (WiFi support)

Make dependencies
-----------------
* automoc4
* cmake

Installation
------------
* download sources
* extract it and install the application:

        cd /path/to/extracted/archive
        mkdir build && cd build
        cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ../
        make
        sudo make install

  If you want install it without KDE widget just run:

        cd /path/to/extracted/archive
        mkdir build && cd build
        cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_DATAENGINE:BOOL=0 -DBUILD_PLASMOID:BOOL=0 ../
        make
        sudo make install

Additional information
======================

Links
-----
* [Homepage](http://arcanis.name/projects/netctl-gui)
* Archlinux [AUR](https://aur.archlinux.org/packages/netctl-gui) package
