# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

pkgname=kdeplasma-applets-netctl-monitor
_pkgname=netctl-monitor
pkgver=1.0.0
pkgrel=1
pkgdesc="Plasmoid written on C++ which interacts with netctl"
arch=('i686' 'x86_64')
url="http://arcan1s.github.io/projects/netctlmonitor"
license=('GPLv3')
depends=('kdebase-workspace')
makedepends=('cmake' 'automoc4')
source=(https://github.com/arcan1s/netctlmonitor/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('d3ab03ddea1e4793cfc5f35a0f7a5ff1')
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
