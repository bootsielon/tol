#! /bin/bash
#

../configure --with-tcl=/usr/lib/tcl8.4 --with-hashmap=google --enable-DS --enable-OIS --enable-ZipArchive --enable-symbols --enable-pool ${1+"$@"}