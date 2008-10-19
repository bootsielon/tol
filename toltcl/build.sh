autoconf
cd release
./default_config.sh --with-tol=/usr/local/tol-gcc-release
make
sudo make install
cd ..
cd debug
./default_config.sh --with-tol=/usr/local/tol-gcc-debug
make
sudo make install
