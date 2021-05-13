include($$PWD/asio.pri)

TARGET = $${ASIO_LIB_NAME}$${FILE_SUFFIX}
TEMPLATE = lib

SOURCES += \
    $$ASIO_SOURCES \

HEADERS += \
    $$ASIO_HEADERS \

asio_headers.path = $${OUTPUT_INCLUDE_ASIO_PATH}
asio_headers.files = $${ASIO_HEADERS}
INSTALLS += asio_headers

target.path = $${OUTPUT_LIBRARY_PATH}
#target.commands += sudo ldconfig

INSTALLS += target

QMAKE_CLEAN += \
  $${asio_headers}/*.h \
  $${OUTPUT_LIBRARY_PATH}/lib$${ASIO_LIB_NAME}*
