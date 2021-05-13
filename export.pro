TEMPLATE = subdirs

lib_tools.subdir      = $$PWD/tools
examples.subdir       = $$PWD/examples

SUBDIRS += \
    lib_tools \
    examples \

lib_tools.CONFIG      = recursive
examples.CONFIG       = recursive

examples.depends      = lib_tools

RESOURCES += \
    $${lib_tools.subdir}/ui/widgets/images.qrc

