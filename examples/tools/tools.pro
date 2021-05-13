TEMPLATE = subdirs

exmpl_asio.subdir = $$PWD/asio

SUBDIRS += \
    exmpl_asio \

exmpl_asio.CONFIG = recursive

QMAKE_EXTRA_TARGETS += \
    $$PWD/../../tools \

