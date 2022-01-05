# Install script for directory: /root/learn/my_naruto

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/async_file_appender.hpp;/usr/local/include/blog.hpp;/usr/local/include/file_writer_type.hpp;/usr/local/include/log_appender_interface.hpp;/usr/local/include/log_buffer.hpp;/usr/local/include/log_config.hpp;/usr/local/include/log_file.hpp;/usr/local/include/log_level.hpp;/usr/local/include/logger.hpp;/usr/local/include/condition.hpp;/usr/local/include/count_down_latch.hpp;/usr/local/include/mutex.hpp;/usr/local/include/singleton.hpp;/usr/local/include/thread.hpp;/usr/local/include/timestamp.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include" TYPE FILE FILES
    "/root/learn/my_naruto/include/async_file_appender.hpp"
    "/root/learn/my_naruto/include/blog.hpp"
    "/root/learn/my_naruto/include/file_writer_type.hpp"
    "/root/learn/my_naruto/include/log_appender_interface.hpp"
    "/root/learn/my_naruto/include/log_buffer.hpp"
    "/root/learn/my_naruto/include/log_config.hpp"
    "/root/learn/my_naruto/include/log_file.hpp"
    "/root/learn/my_naruto/include/log_level.hpp"
    "/root/learn/my_naruto/include/logger.hpp"
    "/root/learn/my_naruto/include/util/condition.hpp"
    "/root/learn/my_naruto/include/util/count_down_latch.hpp"
    "/root/learn/my_naruto/include/util/mutex.hpp"
    "/root/learn/my_naruto/include/util/singleton.hpp"
    "/root/learn/my_naruto/include/util/thread.hpp"
    "/root/learn/my_naruto/include/util/timestamp.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/root/learn/my_naruto/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
