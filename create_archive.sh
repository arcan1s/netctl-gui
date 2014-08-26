#!/bin/bash

ARCHIVE="netctl-gui"
SRCDIR="sources"
FILES="AUTHORS CHANGELOG COPYING README.md"
IGNORELIST="build *.qm *.cppcheck .git*"
VERSION=$(grep -m1 PROJECT_VERSION_MAJOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_MINOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_PATCH sources/CMakeLists.txt | awk '{print $3}' | cut -c 1)
# update submodules
git submodule update --init --recursive
# create archive
[[ -e ${ARCHIVE}-${VERSION}-src.tar.xz ]] && rm -f "${ARCHIVE}-${VERSION}-src.tar.xz"
[[ -d ${ARCHIVE} ]] && rm -rf "${ARCHIVE}"
cp -r "${SRCDIR}" "${ARCHIVE}"
for FILE in ${FILES[*]}; do cp -r "$FILE" "${ARCHIVE}"; done
for FILE in ${IGNORELIST[*]}; do find "${ARCHIVE}" -name "${FILE}" -exec rm -rf {} \;; done
tar cJf "${ARCHIVE}-${VERSION}-src.tar.xz" "${ARCHIVE}"
ln -sf "../${ARCHIVE}-${VERSION}-src.tar.xz" arch
rm -rf "${ARCHIVE}"
# update md5sum
MD5SUMS=$(md5sum ${ARCHIVE}-${VERSION}-src.tar.xz | awk '{print $1}')
sed -i "/md5sums=('[0-9A-Fa-f]*/s/[^'][^)]*/md5sums=('${MD5SUMS}'/" arch/PKGBUILD
sed -i "s/pkgver=[0-9.]*/pkgver=${VERSION}/" arch/PKGBUILD
# clear
find . -type f -name '*src.tar.xz' -not -name "*${VERSION}-src.tar.xz" -exec rm -rf {} \;
find arch -type l -xtype l -exec rm -rf {} \;
