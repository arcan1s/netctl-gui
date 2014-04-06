#!/bin/bash

ARCHIVE="netctl-gui"
SRCDIR="sources"
FILES="AUTHORS CHANGELOG COPYING README.md"
IGNORELIST="gui/resources/translations/english.qm gui/resources/translations/russian.qm"
VERSION=$(grep -m1 PROJECT_VERSION_MAJOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_MINOR sources/CMakeLists.txt | awk '{print $3}' | cut -c 1).\
$(grep -m1 PROJECT_VERSION_PATCH sources/CMakeLists.txt | awk '{print $3}' | cut -c 1)

# create archive
[[ -e ${ARCHIVE}-${VERSION}-src.tar.xz ]] && rm -f "${ARCHIVE}-${VERSION}-src.tar.xz"
[[ -d ${ARCHIVE} ]] && rm -rf "${ARCHIVE}"
cp -r "${SRCDIR}" "${ARCHIVE}"
for FILE in ${FILES[*]}; do cp -r "$FILE" "${ARCHIVE}"; done
for FILE in ${IGNORELIST[*]}; do rm -f "${ARCHIVE}/${FILE}"; done
tar cvJf "${ARCHIVE}-${VERSION}-src.tar.xz" "${ARCHIVE}" > /dev/null
rm -rf "${ARCHIVE}"

# update md5sum
MD5SUMS=$(md5sum ${ARCHIVE}-${VERSION}-src.tar.xz | awk '{print $1}')
sed -i "/md5sums=('[0-9A-Fa-f]*/s/[^'][^)]*/md5sums=('${MD5SUMS}'/" PKGBUILD
sed -i "s/pkgver=[0-9.]*/pkgver=${VERSION}/" PKGBUILD
