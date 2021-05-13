isEmpty(ICM_COMPLETE) : {
  ICM_COMPLETE=$$system('sudo sysctl -w net.ipv4.ping_group_range="0 1001"')
}

include($$PWD/../tools/ui/ui_config.pri)
include($$PWD/../tools/ui/app/app.pri)
include($$PWD/../tools/ui/widgets/widgets.pri)
include($$PWD/../tools/ui/utils/utils.pri)
include($$PWD/../tools/core/asio/asio.pri)

TARGET = $${APP_NAME}$${FILE_SUFFIX}
TEMPLATE = app

SOURCES += *.cpp

target.path=$${OUTPUT_BINARY_EXMPL_PATH}/gui
INSTALLS += target

LIBS += -L$$OUTPUT_LIBRARY_PATH -lspo.core$${FILE_SUFFIX}
LIBS += -L$$OUTPUT_LIBRARY_PATH -lspo.app$${FILE_SUFFIX}
LIBS += -L$$OUTPUT_LIBRARY_PATH -lspo.kuf$${FILE_SUFFIX}
LIBS += -L$$OUTPUT_LIBRARY_PATH -lspo.bzi$${FILE_SUFFIX}
LIBS += -L$$OUTPUT_LIBRARY_PATH -lspo.gui$${FILE_SUFFIX}
LIBS += -L/usr/include/boost -lboost_coroutine

QMAKE_EXTRA_TARGETS += \
    $$PWD/../tools/ui \

QMAKE_DISTCLEAN += \
    $${target.path}/$${TARGET}*

