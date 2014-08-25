Netctl GUI
==========

Information
-----------

Qt4/Qt5 graphical interface for netctl. It may work with profiles and may create new profiles. Also it may create a connection to WiFi. Moreover, it provides a widget and DataEngine for KDE.

**NOTE** [LOOKING FOR TRANSLATORS!](https://github.com/arcan1s/netctl-gui/issues/3)

Configuration
-------------

It is recommended to use graphical interface for widget and DataEngine configuration. Configuration files are:

* `$HOME/.config/netctl-gui.conf` - GUI/helper user configuration
* `/etc/netctl-gui.conf` - helper system-wide configuration
* `$KDEHOME/share/config/netctl.conf` - DataEngine user configuration
* `$KDESYSTEM/share/config/netctl.conf` - DataEngine system-wide configuration

Instruction
===========

Dependencies
------------

* netctl
* qt5-base (if Qt5 is used) or qt4 (if Qt4 is used)

Optional dependencies
---------------------

* kdebase-workspace (widget)
* sudo (sudo support)
* wpa_supplicant (WiFi support)

Make dependencies
-----------------

* automoc4
* cmake
* qt5-tools (if Qt5 is used)

Installation
------------

* download sources
* extract it and install the application:

        cd /path/to/extracted/archive
        mkdir build && cd build
        cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ../
        make
        sudo make install

  Available cmake flags:

    * components:
        * `-DBUILD_DATAENGINE:BOOL=0` - do not build DataEngine
        * `-DBUILD_GUI:BOOL=0` - do not build GUI
        * `-DBUILD_HELPER:BOOL=0` - do not build helper daemon
        * `-DBUILD_LIBRARY:BOOL=0` - do not build library
        * `-DBUILD_PLASMOID:BOOL=0` - do not build Plasmoid
    * additional components
        * `-DBUILD_DOCS:BOOL=0` - do not build developer documentation
        * `-DBUILD_TEST:BOOL=1` - build auto tests for the library and the helper
    * project properties
        * `-DDBUS_SYSTEMCONF_PATH=/etc/dbus-1/system.d/` - path to dbus system configuration files
        * `-DSYSTEMD_SERVICE_PATH=lib/systemd/system` - path to systemd services
        * `-DUSE_CAPABILITIES:BOOL=0` - do not use setcap to get privileges to the helper
        * `-DUSE_QT5:BOOL=0` - use Qt4 instead of Qt5 for GUI

Additional information
======================

TODO (wish list)
----------------

Links
-----

* [Homepage](http://arcanis.name/projects/netctl-gui)
* Page on [kde-apps](http://kde-apps.org/content/show.php?content=164490)
* Archlinux [AUR](https://aur.archlinux.org/pkgbase/netctl-gui/) package
