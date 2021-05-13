include($$PWD/../core_config.pri)

OUTPUT_CORE_WATCH_PATH = $$OUTPUT_LIBRARY_PATH
OUTPUT_INCLUDE_CORE_WATCH_PATH = $$OUTPUT_INCLUDE_PATH/core/watchers

WATCH_INCLUDE = $$INPUT_INCLUDE_PATH/core/watchers
WATCH_SOURCES = $$PWD/src/*.cpp
WATCH_HEADERS = $$WATCH_INCLUDE/*.h

!contains(LIBS,inotifytools) : {
  LIBS += -L/usr/lib -linotifytools
}

isEmpty(INOTIFY_COMPLETE) : {
  INOTIFY_COMPLETE=$$system('sudo sysctl -w fs.inotify.max_user_watches=65536')
}
