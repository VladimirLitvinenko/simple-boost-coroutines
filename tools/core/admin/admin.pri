include($$PWD/../utils/utils.pri)

ADM_HEADERS_PATH = \
    $${INPUT_INCLUDE_PATH}/core/admin \

ADM_HEADERS = \
    $${ADM_HEADERS_PATH}/*.h \
#    $${INPUT_INCLUDE_PATH}/core/utils/*.h \

ADM_SOURCES = \
    $$PWD/src/*.cpp \

#LIBS += -lcrypt

