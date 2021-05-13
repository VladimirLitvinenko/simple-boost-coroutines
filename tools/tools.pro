include( $$PWD/../config.pri)

CONFIG += create_pri
TEMPLATE = subdirs

lib_core.subdir         = $$PWD/core
lib_asio.subdir         = $$PWD/asio

SUBDIRS += \
      lib_core \
      lib_asio \

lib_core.CONFIG         = recursive
lib_asio.CONFIG         = recursive

lib_asio.depands           = lib_core

HEADERS += $$PWD/../include/MainLiterals.h
