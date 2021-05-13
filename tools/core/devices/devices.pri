include($$PWD/../core_config.pri)
include($$PWD/../documents/documents.pri)

OUTPUT_CORE_WATCH_PATH = $$OUTPUT_LIBRARY_PATH
OUTPUT_INCLUDE_CORE_DEVICES_PATH = $$OUTPUT_INCLUDE_PATH/core/devices

DEVICES_INCLUDE = $$INPUT_INCLUDE_PATH/core/devices
DEVICES_SOURCES = $$PWD/src/*.cpp
DEVICES_HEADERS = \
  $${DEVICES_INCLUDE}/*.h \

#unix:!macx: LIBS += -L/lib/x86_64-linux-gnu -lcap
unix:!macx: LIBS += -L/lib/x86_64-linux-gnu -ludev
