# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

# comment out needed line if you dont need this component
#_build_gui=0
#_build_plasmoid=0

pkgname=netctl-gui
pkgver=1.0.0
pkgrel=1
pkgdesc="Qt4 GUI for netctl. Provides a plasmoid for KDE4"
arch=('i686' 'x86_64')
url="http://arcan1s.github.io/projects/netctlplasmoid"
license=('GPLv3')
depends=('netctl' 'qt4')
makedepends=('automoc4' 'cmake' 'desktop-file-utils' 'xdg-utils')
optdepends=('kdebase-runtime: sudo support'
            'sudo: sudo support')
source=("https://github.com/arcan1s/netctlplasmoid/releases/download/V.${pkgver}/${pkgname}-${pkgver}-src.tar.xz")
install="${pkgname}.install"
md5sums=('a8893bf6d745cbcc7e54bab227cc6d5a')

# flags
_cmakekeys="-DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release"
[[ ${_build_gui} == 0 ]] && _cmakekeys=${_cmakekeys}" -DBUILD_GUI:BOOL=0"
[[ ${_build_plasmoid} == 0 ]] && _cmakekeys=${_cmakekeys}" -DBUILD_DATAENGINE:BOOL=0 -DBUILD_PLASMOID:BOOL=0" || depends[1]='kdebase-workspace'

prepare() {
  rm -rf "${srcdir}/build"
  mkdir "${srcdir}/build"
}

build () {
  cd "${srcdir}/build"
  cmake ${_cmakekeys} ../${pkgname}
  make
}

package() {
  cd "${srcdir}/build"
  make DESTDIR="${pkgdir}" install
}
