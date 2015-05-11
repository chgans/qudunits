#!/bin/sh

find . \( -name Makefile -o -name '*.moc' -o -name '*.o' -o -name '*.moc' -o -name '*.so*' \) -exec rm -f {} \;
find . \( -name '*.gc*' -o -name '*.info' \) -exec rm -f {} \;
rm -f tests/tst_udunits2

qmake "QMAKE_CXXFLAGS+= -g -O0 --coverage" "QMAKE_LFLAGS+=--coverage" -recursive
make

cd src
TMPINFO=$PWD/tmp.info
INFO=$PWD/qudunit.info
lcov --directory $PWD \
     --base-directory $PWD \
     --output-file $TMPINFO \
     --capture \
     --initial \
     --ignore-errors source;

export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
../tests/tst_udunits2

# Capture coverage info
lcov --directory $PWD \
     --base-directory $PWD \
     --output-file $TMPINFO \
     --capture \
     --ignore-errors source > /dev/null 2>&1;

# Clean system headers
lcov --output-file $INFO \
     --remove $TMPINFO \
     "/usr/include/*" "/usr/include/*/*" \
     "*c++*" "*gcc*" "*Qt*"

# Generate report (TODO: move to gcovr once travis moves to newer ubuntu)
genhtml --output-directory $PWD/lcov \
        --prefix $PWD \
	--demangle-cpp \
	$INFO


