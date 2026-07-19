cmake_minimum_required(VERSION 3.25)
project(grrmod-mpt VERSION 0.0.1)

option(GRRMOD_INSTALL "Generate the install target" ON)
option(GRRMOD_USE_MOD "Enable MOD support" ON)
option(GRRMOD_USE_MP3 "Enable MP3 support" ON)

include(GNUInstallDirs)

find_library(AESND aesnd
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)

find_library(OPENMPT openmpt
  PATHS "${OGC_ROOT}/../portlibs/ppc/lib/"
  REQUIRED
)

if(GRRMOD_USE_MOD)
  file(GLOB MOD_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/GRRMOD_MOD.c"
  )
endif()
if(GRRMOD_USE_MP3)
  file(GLOB MP3_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/GRRMOD_MP3.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/mpg123/*.c"
  )
endif()

add_library(grrmod-mpt STATIC)

target_compile_options(grrmod-mpt
  PRIVATE
  -Wall
  -Wunused
)

target_sources(grrmod-mpt
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/GRRMOD_core.c"
  "${MOD_SRC_FILES}"
  "${MP3_SRC_FILES}"
)

target_compile_options(grrmod-mpt PRIVATE
  -DHAVE_FCNTL_H -DHAVE_INTTYPES_H -DHAVE_LIMITS_H -DHAVE_MALLOC_H
  -DHAVE_MEMCMP -DHAVE_SETENV -DHAVE_SNPRINTF -DHAVE_STDINT_H
  -DHAVE_STDLIB_H -DHAVE_STRCASECMP -DHAVE_STRDUP -DHAVE_STRINGS_H
  -DHAVE_STRING_H -DHAVE_STRSTR -DHAVE_SYS_STAT_H -DHAVE_SYS_TYPES_H
  -DHAVE_SYS_WAIT_H -DHAVE_UNISTD_H -DHAVE_ATOLL -DHAVE_LANGINFO_H
  -DHAVE_LIBM -DHAVE_LOCALE_H -DHAVE_NL_LANGINFO -DHAVE_SIGNAL_H
  -DHAVE_STDIO_H -DHAVE_SYS_SIGNAL_H -DHAVE_SYS_PARAM_H -DHAVE_STRERROR
  -DHAVE_SYS_RESOURCE_H
)
if(GRRMOD_USE_MP3)
  target_compile_options(grrmod-mpt PRIVATE
    -DOPT_GENERIC
    -DREAL_IS_FLOAT
  )
endif()

target_include_directories(grrmod-mpt
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/GRRMOD"
  "$<BUILD_INTERFACE:${OGC_ROOT}/../portlibs/ppc/include>"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/mpg123"
)

target_link_libraries(grrmod-mpt
  PUBLIC
  ${AESND}
  ${OPENMPT}
)

if(GRRMOD_INSTALL)

  configure_file(grrmod-mpt.pc.in ${CMAKE_BINARY_DIR}/grrmod-mpt.pc @ONLY)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/grrmod-mpt.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(FILES ${CMAKE_BINARY_DIR}/grrmod-mpt.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )

  install(
    TARGETS grrmod-mpt EXPORT grrmod-mpt-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

endif()
