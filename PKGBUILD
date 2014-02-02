# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

_build_gui=0
_build_plasmoid=1

pkgname=kdeplasma-applets-netctl
_pkgname=netctl-plasmoid
pkgver=1.0.0
pkgrel=1
pkgdesc="Plasmoid written on C++ which interacts with netctl"
arch=('i686' 'x86_64')
url="http://arcan1s.github.io/projects/netctlplasmoid"
license=('GPLv3')
depends=('netctl' 'qt4')
makedepends=('cmake' 'automoc4')
optdepends=('kdebase-runtime: sudo support'
            'sudo: sudo support')
source=(https://github.com/arcan1s/netctlplasmoid/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz)
install=${pkgname}.install
md5sums=('1faefe9a5491837863b08fcce79accfe')
_cmakekeys="-DCMAKE_INSTALL_PREFIX=$(kde4-config --prefix) -DCMAKE_BUILD_TYPE=Release"

prepare() {
  # flags
  [[ ${_build_gui} == 0 ]] && _cmakekeys=${_cmakekeys}" -DBUILD_GUI:BOOL=0"
  [[ ${_build_plasmoid} == 1 ]] && depends+=('kdebase-workspace') || _cmakekeys=${_cmakekeys}" -DBUILD_PLASMOID:BOOL=0"
  
  # build directory
  [[ -d ${srcdir}/build ]] && rm -rf "${srcdir}/build"
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
