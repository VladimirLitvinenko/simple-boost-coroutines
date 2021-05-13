include($$PWD/SospoAdmin.pri)

TARGET = SospoAdmin$$FILE_SUFFIX
TEMPLATE = lib

HEADERS = $$ADM_HEADERS
SOURCES = $$ADM_SOURCES

target.path = $${OUTPUT_CORE_PATH}

admin_headers.path  = $${OUTPUT_INCLUDE_CORE_PATH}
admin_headers.files = \
  $${ADM_HEADERS} \

utils_headers.path = $${OUTPUT_INCLUDE_CORE_PATH}/utils
utils_headers.files = $$PWD/utils/*.h

INSTALLS += \
    utils_headers \
    admin_headers \
    target
