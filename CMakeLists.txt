cmake_minimum_required(VERSION 3.7.2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(Colours)
include(Platform)

project(cellar CXX)
string(TIMESTAMP BUILD_DATE "%Y.%m.%d %H:%M:%S UTC" UTC)
# local cmake modules

# for installation rules, from CMake wiki
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/cellar")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(Git)
git_init()

include(Cog)
cog_target()

include(Ronn)

generate_manpage(cellar 1)
add_manpage_target()

find_package(Boost 1.63 REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(TCLAP "tclap >= 1.2.1")
if(NOT TCLAP_FOUND)
MESSAGE(WARNING "TCLAP is required by this project, but was not found by CMake.
Continuing on the assumption that you've downloaded it from
http://tclap.sourceforge.net and put the headers in ./include
(wink, nudge)")
endif(NOT TCLAP_FOUND)

#include(LavaTargets)
include(Binaries)

include_directories(include)
configure_file("${CMAKE_SOURCE_DIR}/include/cmake.hpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/cmake.hpp")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")
set(src "${CMAKE_SOURCE_DIR}/src")

build_library(TARGET libcellar
        SUBDIRS bottles config launch paths
        DEPENDS cog)
set_target_properties(libcellar PROPERTIES PREFIX "")

build_executable(TARGET cellar
        SUBDIRS core help
        LIBRARIES libcellar Boost::filesystem Boost::system
        DEPENDS cog)

install(TARGETS cellar libcellar 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib/cellar
        ARCHIVE DESTINATION share/cellar)
