cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

project(PrEWUtils) # Project name

# External libraries
# TODO TODO TODO MAKE THIS PATH UNSPECIFIC!
find_library( 
  PrEW PrEW 
  HINTS 
    ../PrEW/lib # On my local computer
    /afs/desy.de/group/flc/pool/beyerjac/TGCAnalysis/PrEW/lib # On the NAF
  REQUIRED 
)
find_library( 
  spdlog spdlog 
  HINTS 
    ../PrEW/external/spdlog/build 
    /afs/desy.de/group/flc/pool/beyerjac/TGCAnalysis/PrEW/PrEW/external/spdlog/build 
  REQUIRED 
)

# Find installation directories and set include paths accordingly
get_filename_component(PrEW_lib_dir ${PrEW} DIRECTORY)
get_filename_component(spdlog_lib_dir ${spdlog} DIRECTORY)
include_directories(
  ${PrEW_lib_dir}/../source/prew/include
  ${spdlog_lib_dir}/../include
)

# Common packages
find_package( ROOT 6.16 REQUIRED ) 
include_directories( SYSTEM ${ROOT_INCLUDE_DIRS} )
link_libraries( ${ROOT_LIBRARIES} )
add_definitions( ${ROOT_DEFINITIONS} )

# Local header files that can be included using #include
include_directories(
  source/include
)

# Code subdirectories with own CMakelists
add_subdirectory(source)

# Install in local folder instead of system
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# Package name and version
set(CPACK_PACKAGE_NAME "PrEWUtils")
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)