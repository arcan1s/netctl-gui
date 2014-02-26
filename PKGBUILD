# Maintainer: Evgeniy "arcanis" Alexeev <arcanis.arch at gmail dot com>

# comment out needed line if you dont need this component
#_build_gui=0
#_build_plasmoid=0

pkgname=kdeplasma-applets-netctl
_pkgname=netctl-plasmoid
pkgver=1.0.0
pkgrel=1
pkgdesc="Plasmoid with Qt4 GUI which interacts with netctl"
arch=('i686' 'x86_64')
url="http://arcan1s.github.io/projects/netctlplasmoid"
license=('GPLv3')
depends=('netctl' 'qt4')
makedepends=('cmake' 'automoc4')
optdepends=('kdebase-runtime: sudo support'
            'sudo: sudo support')
source=("https://github.com/arcan1s/netctlplasmoid/releases/download/V.${pkgver}/${_pkgname}-${pkgver}-src.tar.xz")
install="${pkgname}.install"
md5sums=('115f410536dc2c09529853a923e8e34c')

# flags
_cmakekeys="-DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release"
[[ ${_build_gui} == 0 ]] && _cmakekeys=${_cmakekeys}" -DBUILD_GUI:BOOL=0"
[[ ${_build_plasmoid} == 0 ]] && _cmakekeys=${_cmakekeys}" -DBUILD_DATAENGINE:BOOL=0 -DBUILD_PLASMOID:BOOL=0" || depends[1]='kdebase-workspace'

prepare() {
  # build directory
  rm -rf "${srcdir}/build"
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
