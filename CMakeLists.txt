cmake_minimum_required (VERSION 3.0)

project (my_naruto)

include_directories (include)

aux_source_directory (include INCLUDE_SRCS)

file (GLOB_RECURSE HEADERS *hpp)


set(CXX_FLAGS
 -O2
 -g
 -Wall
 -Wextra
 -Werror
 -std=c++14
 -pthread
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ---[ Flags
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

# add_library (mynaruto ${INCLUDE_SRCS})

# install (TARGETS mynaruto DESTINATION /usr/local/bin)
install (FILES ${HEADERS} DESTINATION /usr/local/include)