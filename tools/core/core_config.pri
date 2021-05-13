include($$PWD/../../config.pri)


QT += core network

CORE_LIB_NAME = example.core

#FLY += core ui

DEFINES += SOSPO_CORE_LIBRARY

OUTPUT_CORE_PATH         = $$OUTPUT_LIBRARY_PATH
OUTPUT_INCLUDE_CORE_PATH = $$OUTPUT_INCLUDE_PATH/core
