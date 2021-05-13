include($$PWD/core_config.pri)

TARGET = $${CORE_LIB_NAME}$${FILE_SUFFIX}
TEMPLATE = lib

include($$PWD/admin/admin.pri)
include($$PWD/documents/documents.pri)
include($$PWD/watchers/watchers.pri)
include($$PWD/utils/utils.pri)

SOURCES += \
    $$ADM_SOURCES \
    $$UTILS_SOURCES \
    $$DOCS_SOURCES \
    $$WATCH_SOURCES \

HEADERS += \
    $$ADM_HEADERS \
    $$UTILS_HEADERS \
    $$DOCS_HEADERS \
    $$WATCH_HEADERS \

core_headers_literal.path = $${OUTPUT_INCLUDE_PATH}
core_headers_literal.files = $${INPUT_INCLUDE_PATH}/*.h
INSTALLS += core_headers_literal

core_headers_admin.path = $${OUTPUT_INCLUDE_CORE_PATH}/admin
core_headers_admin.files = $${ADM_HEADERS}
INSTALLS += core_headers_admin

core_headers_utils.path = $${OUTPUT_INCLUDE_CORE_PATH}/utils
core_headers_utils.files = $${UTILS_HEADERS}
INSTALLS += core_headers_utils

core_headers_docs.path = $${OUTPUT_INCLUDE_CORE_DOCS_PATH}
core_headers_docs.files = $${DOCS_HEADERS}
INSTALLS += core_headers_docs

core_headers_watch.path = $${OUTPUT_INCLUDE_CORE_WATCH_PATH}
core_headers_watch.files = $${WATCH_HEADERS}
INSTALLS += core_headers_watch

target.path = $${OUTPUT_LIBRARY_PATH}
#target.commands += sudo ldconfig

INSTALLS += target

QMAKE_CLEAN += \
  $${core_headers_literal}/*.h \
  $${core_headers_admin.path}/*.h \
  $${core_headers_utils.path}/*.h \
  $${core_headers_docs.path}/*.h \
  $${core_headers_watch.path}/*.h \
  $${OUTPUT_LIBRARY_PATH}/lib$${CORE_LIB_NAME}*

