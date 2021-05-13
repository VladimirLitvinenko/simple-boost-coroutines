
ADMIN_PATH = $$PWD/build/usr/local/export

OUTPUT_LIBRARY_PATH     = $${ADMIN_PATH}/lib
OUTPUT_BINARY_EXMPL_PATH= $${ADMIN_PATH}/examples
OUTPUT_INCLUDE_PATH     = $${ADMIN_PATH}/include
OUTPUT_CONFIG_PATH      = $${ADMIN_PATH}/conf

system('mkdir -p $$OUTPUT_LIBRARY_PATH')
system('mkdir -p $$OUTPUT_BINARY_EXMPL_PATH')
system('mkdir -p $$OUTPUT_INCLUDE_PATH')
system('mkdir -p $$OUTPUT_CONFIG_PATH')

INPUT_INCLUDE_PATH = $$PWD/include
INCLUDEPATH += $${INPUT_INCLUDE_PATH}

#isEmpty($$find(QMAKE_CXXFLAGS,fPIC)): QMAKE_CXXFLAGS+= -fPIC

!contains(QMAKE_CXXFLAGS,std): QMAKE_CXXFLAGS += -std=c++11
!contains(QMAKE_CXXFLAGS,Wall): QMAKE_CXXFLAGS += -Wall
!contains(QMAKE_CFLAGS,std): QMAKE_CFLAGS += -std=c++11
!contains(DEFINES,QT_DEPRECATED_WARNINGS): DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += exceptions
CONFIG += c++11

DEFINES += BOOST_COROUTINES_NO_DEPRECATION_WARNING

isEmpty($$find(QT,core)): QT += core
isEmpty($$find(QT,network)): QT += network
isEmpty($$find(QT,xml)): QT += xml

FILE_SUFFIX=
CONFIG(debug,debug|release): FILE_SUFFIX=-dbg

VER_MAJ=1
VER_MIN=0
VER_PAT=0
VERSION=$${VER_MAJ}.$${VER_MIN}.$${VER_PAT}


