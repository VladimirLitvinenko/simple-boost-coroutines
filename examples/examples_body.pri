
include($$PWD/../tools/core/core_config.pri)
include($$PWD/../tools/core/admin/admin.pri)
include($$PWD/../tools/core/watchers/watchers.pri)
include($$PWD/../tools/asio/asio.pri)

system('sudo echo "$$OUTPUT_LIBRARY_PATH" > etc/ld.so.conf.d/examples.conf')
system('sudo ldconfig')

TARGET = $${APP_NAME}$${FILE_SUFFIX}

CONFIG += console
CONFIG -= app_bundle

SOURCES += *.cpp

target.path=$${OUTPUT_BINARY_EXMPL_PATH}

INSTALLS += target

LIBS += -L$$OUTPUT_LIBRARY_PATH -l$${CORE_LIB_NAME}$${FILE_SUFFIX}
LIBS += -L$$OUTPUT_LIBRARY_PATH -l$${ASIO_LIB_NAME}$${FILE_SUFFIX}

LIBS += -L/usr/include/boost -lboost_coroutine\

LIBS += \
    -L/usr/include/boost \
    -lboost_context \
    -lboost_thread \
    -lboost_date_time \
    -lboost_log \
    -lboost_system \

QMAKE_EXTRA_TARGETS += \
    $$PWD/../tools \
    $$PWD/../tools/core \
    $$PWD/../tools/asio \

QMAKE_DISTCLEAN += \
    $${target.path}/$${TARGET}*

