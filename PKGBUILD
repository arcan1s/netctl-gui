# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

pkgbase=netctl-gui
pkgname=('libnetctlgui' 'netctl-gui' 'netctl-gui-qt4' 'kdeplasma-applets-netctl-gui')
pkgver=1.2.0
pkgrel=1
pkgdesc="Qt4/Qt5 GUI for netctl. Also provides a widget for KDE"
arch=('i686' 'x86_64')
url="http://arcanis.name/projects/netctl-gui"
license=('GPL3')
makedepends=('automoc4' 'cmake' 'kdelibs' 'qt5-base' 'qt5-tools')
optdepends=('kdebase-runtime: sudo support'
            'kdeplasma-applets-netctl-gui: KDE widget'
            'sudo: sudo support'
            'wpa_supplicant: wifi support')
source=("https://github.com/arcan1s/netctl-gui/releases/download/V.${pkgver}/${pkgbase}-${pkgver}-src.tar.xz")
install="${pkgbase}.install"
md5sums=('2290c8a47dceeda69e523143d82680d4')


prepare() {
  rm -rf "${srcdir}/build-"{plasmoid,qt4,qt5}
  mkdir "${srcdir}/build-"{plasmoid,qt4,qt5}
}


build() {
  cd "${srcdir}/build-plasmoid"
  cmake -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_DOCS:BOOL=0 \
        -DBUILD_GUI:BOOL=0 \
        -DBUILD_LIBRARY:BOOL=0 \
        "../${pkgbase}"
  make

  cd "${srcdir}/build-qt4"
  cmake -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_DOCS:BOOL=0 \
        -DBUILD_DATAENGINE:BOOL=0 \
        -DBUILD_PLASMOID:BOOL=0 \
        -DUSE_QT5:BOOL=0 \
        "../${pkgbase}"
  make

  cd "${srcdir}/build-qt5"
  cmake -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_DOCS:BOOL=0 \
        -DBUILD_DATAENGINE:BOOL=0 \
        -DBUILD_PLASMOID:BOOL=0 \
        "../${pkgbase}"
  make
}


package_libnetctlgui() {
  pkgdesc="Qt5 library which interacts with netctl. A part of netctl-gui"
  depends=('netctl' 'qt5-base')

  cd "${srcdir}/build-qt5/netctlgui"
  make DESTDIR="${pkgdir}" install
}


package_kdeplasma-applets-netctl-gui() {
  pkgdesc="A plasmoid, which interacts with netctl. A part of netctl-gui"
  depends=('netctl' 'kdebase-workspace')
  optdepends=('kdebase-runtime: sudo support'
              'netctl-gui: graphical front-end'
              'netctl-gui-qt4: graphical front-end'
              'sudo: sudo support')
  install="${pkgbase}.install"

  cd "${srcdir}/build-plasmoid"
  make DESTDIR="${pkgdir}" install
}


package_netctl-gui() {
  pkgdesc="Qt5 graphical front-end for netctl. A part of netctl-gui"
  depends=('libnetctlgui')
  provides=('netctl-gui-qt4')
  conflicts=('netctl-gui-qt4')
  install="${pkgbase}.install"

  cd "${srcdir}/build-qt5/gui"
  make DESTDIR="${pkgdir}" install
}


package_netctl-gui-qt4() {
  pkgdesc="Qt4 graphical front-end for netctl. A part of netctl-gui"
  depends=('netctl' 'qt4')
  provides=('libnetctlgui' 'netctl-gui')
  conflicts=('libnetctlgui' 'netctl-gui')
  install="${pkgbase}.install"

  cd "${srcdir}/build-qt4"
  make DESTDIR="${pkgdir}" install
}
