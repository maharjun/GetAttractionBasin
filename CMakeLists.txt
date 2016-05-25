cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
include(CppGlob)
include(CustomMexFunctions)
project(GetAttractionBasin)

FIND_PACKAGE(Matlab REQUIRED)

message(STATUS "C Compiler   = ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler = ${CMAKE_CXX_COMPILER}")

# define submodules directory and adding into include path
   if(NOT DEFINED ENV{SUBMODULES_DIR} AND NOT DEFINED SUBMODULES_DIR)
      set(SUBMODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR})
   elseif(NOT DEFINED SUBMODULES_DIR)
      set(SUBMODULES_DIR $ENV{SUBMODULES_DIR})
   endif()
   message(STATUS "SUBMODULES_DIR = ${SUBMODULES_DIR}")
   include_directories(BEFORE ${SUBMODULES_DIR})

# configure the dynamical system include file
   if(NOT DYN_SYSTEM_HPP_PATH)
      set(DYN_SYSTEM_HPP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ExampleDynSys/IzhikevichSpiking.hpp)
   endif()

   if(NOT PREV_DYN_SYSTEM_FILE OR NOT PREV_DYN_SYSTEM_FILE STREQUAL DYN_SYSTEM_HPP_PATH)
      get_filename_component(DYN_SYSTEM_FILE_NAME ${DYN_SYSTEM_HPP_PATH} NAME)

      string(REGEX REPLACE "^([^\\.]*)\\..*$" "\\1" DYN_SYSTEM_NAME_LOCAL ${DYN_SYSTEM_FILE_NAME})
      string(MAKE_C_IDENTIFIER DYN_SYSTEM_NAME_LOCAL ${DYN_SYSTEM_NAME_LOCAL})

      set(DYN_SYSTEM_NAME ${DYN_SYSTEM_NAME_LOCAL} CACHE INTERNAL "Stores the class name corresponding to te dynamical system")

      # setting the vars used in the in file
      set(DYNAMICAL_SYSTEM_FILE_PATH ${DYN_SYSTEM_HPP_PATH})
      set(DYNAMICAL_SYSTEM ${DYN_SYSTEM_NAME})

      # configuring the in file
      configure_file(
         Source/DynamicalSystemInclude.in
         DynamicalSystemInclude.hpp)

      # updating PREV_DYN_SYSTEM_FILE
      set(PREV_DYN_SYSTEM_FILE ${DYN_SYSTEM_HPP_PATH} CACHE INTERNAL "Stores the previously used Dynamical System")
   endif()
   unset(DYN_SYSTEM_HPP_PATH CACHE)

# adding the build directory to include directories
   include_directories(AFTER ${CMAKE_BINARY_DIR})

# include the relevant source and header files
   get_cxx_files(GRID2D_HEADER_FILE_PATHS ${SUBMODULES_DIR}/Grid2D/Headers)
   get_cxx_files(GRID2D_MEX_HEADER_FILE_PATHS ${SUBMODULES_DIR}/Grid2D/MexHeaders)
   get_cxx_files(MEXMEM_HEADER_FILE_PATHS ${SUBMODULES_DIR}/MexMemoryInterfacing/Headers)
   get_cxx_files(HEADER_FILE_PATHS Headers)
   get_cxx_files(EXAMPLE_DYN_SYS_HEADERS ExampleDynSys)

   set(COMMON_SOURCE_FILES
         ${GRID2D_HEADER_FILE_PATHS}
         ${GRID2D_MEX_HEADER_FILE_PATHS}
         ${MEXMEM_HEADER_FILE_PATHS}
         ${HEADER_FILE_PATHS})
   set(SOURCE_FILES_1 Source/GetAttrBasinMexMain.cpp
         ${CMAKE_BINARY_DIR}/DynamicalSystemInclude.hpp
         ${EXAMPLE_DYN_SYS_HEADERS}
         ${COMMON_SOURCE_FILES})
   set(SOURCE_FILES ${SOURCE_FILES_1})

   # source group information to generate filters for VS
   source_group(
      "Header Files\\MexMemoryInterfacing" 
      FILES ${MEXMEM_HEADER_FILE_PATHS})
   source_group(
      "Header Files\\Grid2D" 
      FILES ${GRID2D_HEADER_FILE_PATHS})
   source_group(
      "Header Files\\Grid2D\\MexHeaders"
      FILES ${GRID2D_MEX_HEADER_FILE_PATHS})
   source_group(
      "Header Files\\GetAttractionBasin"
      FILES ${HEADER_FILE_PATHS})

# include mex file along with documentation if exists
   get_filename_component(MEX_DOC_SRC_PATH ${PREV_DYN_SYSTEM_FILE} DIRECTORY)
   set(MEX_DOC_SRC_PATH ${MEX_DOC_SRC_PATH}/${DYN_SYSTEM_NAME}.txt)
   message(STATUS "MEX_DOC_SRC_PATH = ${MEX_DOC_SRC_PATH}")
   if (NOT EXISTS ${MEX_DOC_SRC_PATH})
      set(MEX_DOC_SRC_PATH "")
   endif()
   message(STATUS "MEX_DOC_SRC_PATH = ${MEX_DOC_SRC_PATH}")
   custom_add_mex(GetAttBasin_${DYN_SYSTEM_NAME} 
      ${SOURCE_FILES_1}
      DOCUMENTATION ${MEX_DOC_SRC_PATH})

# assign default build type in case of single config generator 
   if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
      message(STATUS "CMAKE_BUILD_TYPE not defined")
      set(CMAKE_BUILD_TYPE "Release")
   endif()
   message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# definition of build type specific parameters
# for DEBUG CONFIG
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DIS_DEBUG=true")
# for RELEASE CONFIG
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DIS_DEBUG=false")

# install instructions
   
   install(DIRECTORY DESTINATION "${CMAKE_BINARY_DIR}/install" CONFIGURATIONS "Release")
   set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
   install_mex(GetAttBasin_${DYN_SYSTEM_NAME} .)
   
# unset some cached variables so that they are restord to their
# default values the next time cmake is run
   unset(SUBMODULES_DIR CACHE)
