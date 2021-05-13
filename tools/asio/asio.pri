
ASIO_LIB_NAME = example.asio

include($$PWD/../core/core_config.pri)
include($$PWD/../core/utils/utils.pri)
include($$PWD/../core/admin/admin.pri)
include($$PWD/../core/documents/documents.pri)

DEFINES+=BOOST_COROUTINE_NO_DEPRECATION_WARNING

isEmpty(ICM_COMPLETE) : {
  ICM_COMPLETE = $$system('sudo iptables -p icmp -h')
  ICM_COMPLETE = $$system('sudo sysctl -w net.ipv4.ping_group_range="0 1010"')
}

isEmpty($$find(QT,network)): QT += network

OUTPUT_ASIO_PATH = $$OUTPUT_LIBRARY_PATH
OUTPUT_INCLUDE_ASIO_PATH = $$OUTPUT_INCLUDE_PATH/asio

ASIO_HEADERS_PATH = \
    $${INPUT_INCLUDE_PATH}/asio \

ASIO_HEADERS = \
    $${ASIO_HEADERS_PATH}/*.h \

ASIO_SOURCES = \
    $$PWD/src/*.cpp \

QMAKE_CLEAN += \
  $${OUTPUT_INCLUDE_ASIO_PATH}/*.h \

isEmpty(BOOST_ADDED) : {
  BOOST_ADDED=1
  INCLUDEPATH += /usr/include/boost
  DEPENDPATH += /usr/include/boost
  LIBS += \
      -lboost_context \
      -lboost_thread \
      -lboost_date_time \
      -lboost_system \
      -lboost_regex \
      -lboost_filesystem \
#      -lboost_coroutine \

}
