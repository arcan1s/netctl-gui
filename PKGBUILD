# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

pkgname=kdeplasma-applets-netctl
_pkgname=netctl-plasmoid
pkgver=1.0.0
pkgrel=1
pkgdesc="Plasmoid written on C++ which interacts with netctl"
arch=('i686' 'x86_64')
url="http://arcan1s.github.io/projects/netctlplasmoid"
license=('GPLv3')
depends=('kdebase-workspace')
makedepends=('cmake' 'automoc4')
source=(https://github.com/arcan1s/netctlplasmoid/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('5ac750e391936a6f3494a819b7b4d0b2')
_cmakekeys="-DCMAKE_INSTALL_PREFIX=$(kde4-config --prefix)
            -DBUILD_GUI:BOOL=1
            -DBUILD_PLASMOID:BOOL=1
            -DCMAKE_BUILD_TYPE=Release"

prepare() {
  if [[ -d ${srcdir}/build ]]; then
    rm -rf "${srcdir}/build"
  fi
  mkdir "${srcdir}/build"
}

build () {
  cd "${srcdir}/build"
  cmake ${_cmakekeys} ../${_pkgname}
  make
}

package() {
  cd "${srcdir}/build"
  make DESTDIR="${pkgdir}" install
}
