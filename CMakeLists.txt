#
# CMAKE SETUP
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#
# PROJECT INFO
#
PROJECT(libk8055)
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "4")
SET(VERSION_PATCH "2")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
ADD_DEFINITIONS(-DVERSION="${VERSION}")
#
# CONFIG OPTIONS
#
OPTION(WANT_DEBUG       "build with debug information" OFF)
IF(WANT_DEBUG)
    SET(CMAKE_BUILD_TYPE Debug)
ELSE()
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()
SET(CMAKE_CXX_FLAGS "-O2 -fno-implement-inlines")
SET(CMAKE_CXX_FLAGS_RELEASE "")
SET(CMAKE_CXX_FLAGS_DEBUG "-g ")
#
# COLOR MESSAGES
#
string( ASCII 27 _escape)
function(COLOR_MESSAGE TEXT)
    if(CMAKE_COLOR_MAKEFILE)
        MESSAGE(${TEXT})
    else()
        string(REGEX REPLACE "${_escape}.[0123456789;]*m" "" __TEXT ${TEXT})
        MESSAGE(${__TEXT})
    endif()
endfunction()
#
# HEADER LIBRARY FUNCTIONS
#
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
CHECK_INCLUDE_FILES(usb.h HAVE_USB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(stdio.h HAVE_STDIO_H)
CHECK_INCLUDE_FILES(assert.h HAVE_ASSERT_H)
CHECK_INCLUDE_FILES(math.h HAVE_MATH_H)
IF (HAVE_USB_H)
    ADD_SUBDIRECTORY(libk8055)
ENDIF()
#
FIND_PACKAGE(wxWidgets COMPONENTS core base)
IF(wxWidgets_FOUND)
    ADD_SUBDIRECTORY(k8055gui)
ENDIF()
#
FIND_PACKAGE(SWIG)
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(PythonInterp)
IF(SWIG_FOUND AND PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
    SET(BUILD_PYTHON TRUE)
    ADD_SUBDIRECTORY(pyk8055)
ENDIF()
#
# BUILD SUMMARY
#
COLOR_MESSAGE("\n${_escape}[1;36mBuild Summary${_escape}[0m\n--------------------")
IF (HAVE_USB_H)
    COLOR_MESSAGE("* ${_escape}[1;35mlibusb${_escape}[0m                :   ${_escape}[1;32mfound${_escape}[0m build libk8055")
ELSE()
    COLOR_MESSAGE("* ${_escape}[1;35mlibusb${_escape}[0m                :   ${_escape}[1;31mmissing${_escape}[0m unable to build libk8055")
    MESSAGE( FATAL_ERROR "Abort" )
ENDIF()
IF(wxWidgets_FOUND)
    COLOR_MESSAGE("* ${_escape}[1;35mwxwidget${_escape}[0m              :   ${_escape}[1;32mfound${_escape}[0m build k8055gui")
ELSE()
    COLOR_MESSAGE("* ${_escape}[1;35mwxwidget${_escape}[0m              :   ${_escape}[1;31mmissing${_escape}[0m unable to build k8055gui")
ENDIF()
IF(SWIG_FOUND)
    COLOR_MESSAGE("* ${_escape}[1;35mSWIG${_escape}[0m                  :   ${_escape}[1;32mfound${_escape}[0m")
ELSE()
    COLOR_MESSAGE("* ${_escape}[1;35mSWIG${_escape}[0m                  :   ${_escape}[1;31mmissing${_escape}[0m")
ENDIF()
IF(PYTHONLIBS_FOUND)
    COLOR_MESSAGE("* ${_escape}[1;35mpython libs${_escape}[0m           :   ${_escape}[1;32mfound${_escape}[0m")
ELSE()
    COLOR_MESSAGE("* ${_escape}[1;35mpython libs${_escape}[0m           :   ${_escape}[1;31mmissing${_escape}[0m")
ENDIF()
IF(PYTHONINTERP_FOUND)
    COLOR_MESSAGE("* ${_escape}[1;35mpython interpreter${_escape}[0m    :   ${_escape}[1;32mfound${_escape}[0m")
ELSE()
    COLOR_MESSAGE("* ${_escape}[1;35mpython interpreter${_escape}[0m    :   ${_escape}[1;31mmissing${_escape}[0m")
ENDIF()
IF(BUILD_PYTHON)
    COLOR_MESSAGE("*                                 build pyk8055")
ELSE()
    COLOR_MESSAGE("*                                 unable to build pyk8055")
ENDIF()
MESSAGE("")
