include($$PWD/network.pri)

TARGET = SospoNetwork$$FILE_SUFFIX
TEMPLATE = lib

SOURCES = $$NET_SOURCE
HEADERS = $$NET_HEADERS

target.path = $${OUTPUT_CORE_NET_PATH}

core_headers_net.path  = $${OUTPUT_INCLUDE_CORE_NET_PATH}
core_headers_net.files = \
    $$PWD/include/*.h \

INSTALLS += \
    core_headers_net \
    target
