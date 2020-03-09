# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
    isp.h
)

set(SOURCE_FILES
	isp.cpp
)

# Don't change this
add_library(src ${SOURCE_FILES} ${HEADER_FILES})