# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
    image.h
    isp.h
    display.h
)

set(SOURCE_FILES
    image.cpp
    isp.cpp
    display.cpp
)

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})