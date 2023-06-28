BINARIES="fqgrep"
make CFLAGS="-w -O3 -I$PREFIX/include -L$PREFIX/lib"

mkdir -p ${PREFIX}/bin
cp $BINARIES $PREFIX/bin
mkdir -p $PREFIX/doc/fqgrep
cp README.md $PREFIX/doc/fqgrep/