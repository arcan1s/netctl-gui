Netctl GUI
==========

Information
-----------

Qt4/Qt5 graphical interface for netctl. It may work with profiles and may create new profiles. Also it may create a connection to WiFi. Moreover, it provides a widget and DataEngine for KDE.

**NOTE** [LOOKING FOR TRANSLATORS!](https://github.com/arcan1s/netctl-gui/issues/3)

Configuration
-------------

It is recommended to use graphical interface for widget and DataEngine configuration. All settings of GUI are stored in `$HOME/.config/netctl-gui.conf`. It is highly recommended to edit it from graphical interface.

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

  * `-DBUILD_DATAENGINE:BOOL=0` - do not build DataEngine
  * `-DBUILD_DOCS:BOOL=0` - do not build developer documentation
  * `-DBUILD_GUI:BOOL=0` - do not build GUI
  * `-DBUILD_LIBRARY:BOOL=0` - do not build library
  * `-DBUILD_PLASMOID:BOOL=0` - do not build Plasmoid
  * `-DUSE_QT5:BOOL=0` - use Qt4 instead of Qt5 for GUI

Additional information
======================

TODO (wish list)
----------------

* refactoring of GUI according to library changes
* rewrite changelog to more comfortable format

Links
-----

* [Homepage](http://arcanis.name/projects/netctl-gui)
* Page on [kde-apps](http://kde-apps.org/content/show.php?content=164490)
* Archlinux [AUR](https://aur.archlinux.org/pkgbase/netctl-gui/) package
