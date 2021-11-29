cmake_minimum_required(VERSION 3.5)
project(pvr.plutotv)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(RapidJSON 1.1.0 REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${RAPIDJSON_INCLUDE_DIRS})

set(PVRPLUTOTV_SOURCES
                    src/Curl.cpp
                    src/Utils.cpp
                    src/PlutotvData.cpp)

set(PVRPLUTOTV_HEADERS
                    src/Curl.h
                    src/Utils.h
                    src/PlutotvData.h)

addon_version(pvr.plutotv IPTV)
add_definitions(-DIPTV_VERSION=${IPTV_VERSION})

build_addon(pvr.plutotv PVRPLUTOTV DEPLIBS)

include(CPack)
