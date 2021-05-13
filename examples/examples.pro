TEMPLATE = subdirs

exmpl_tools.subdir = $$PWD/tools

exmpl_tools.CONFIG = recursive

SUBDIRS += \
    exmpl_tools \

QMAKE_EXTRA_TARGETS += \
    $$PWD/../tools \
