include($$PWD/../core_config.pri)

OUTPUT_CORE_DOCS_PATH = $$OUTPUT_LIBRARY_PATH
OUTPUT_INCLUDE_CORE_DOCS_PATH = $${OUTPUT_INCLUDE_PATH}/core/documents

DOCS_INCLUDES  = $${INPUT_INCLUDE_PATH}/core/documents

DOCS_SOURCES   = \
      $$PWD/src/*.cpp

DOCS_HEADERS   = \
    $${DOCS_INCLUDES}/*.h

