cmake_minimum_required(VERSION 3.25)
project(grrmod-xmp VERSION 0.0.1)

option(GRRMOD_INSTALL "Generate the install target" ON)
option(GRRMOD_USE_MOD "Enable MOD support" ON)

include(GNUInstallDirs)

find_library(AESND aesnd
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)

find_library(LIBXMP xmp
  PATHS "${OGC_ROOT}/../portlibs/ppc/lib/"
  REQUIRED
)

if(GRRMOD_USE_MOD)
  file(GLOB MOD_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/GRRMOD_MOD.c"
  )
endif()

add_library(grrmod-xmp STATIC)

target_compile_options(grrmod-xmp
  PRIVATE
  -Wall
  -Wunused
)

target_sources(grrmod-xmp
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/GRRMOD_core.c"
  "${MOD_SRC_FILES}"
)

target_compile_options(grrmod-xmp PRIVATE
  -DHAVE_FCNTL_H -DHAVE_INTTYPES_H -DHAVE_LIMITS_H -DHAVE_MALLOC_H
  -DHAVE_MEMCMP -DHAVE_SETENV -DHAVE_SNPRINTF -DHAVE_STDINT_H
  -DHAVE_STDLIB_H -DHAVE_STRCASECMP -DHAVE_STRDUP -DHAVE_STRINGS_H
  -DHAVE_STRING_H -DHAVE_STRSTR -DHAVE_SYS_STAT_H -DHAVE_SYS_TYPES_H
  -DHAVE_SYS_WAIT_H -DHAVE_UNISTD_H -DHAVE_ATOLL -DHAVE_LANGINFO_H
  -DHAVE_LIBM -DHAVE_LOCALE_H -DHAVE_NL_LANGINFO -DHAVE_SIGNAL_H
  -DHAVE_STDIO_H -DHAVE_SYS_SIGNAL_H -DHAVE_SYS_PARAM_H -DHAVE_STRERROR
  -DHAVE_SYS_RESOURCE_H
)

target_include_directories(grrmod-xmp
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/GRRMOD"
  "$<BUILD_INTERFACE:${OGC_ROOT}/../portlibs/ppc/include>"
)

target_link_libraries(grrmod-xmp
  PUBLIC
  ${AESND}
  ${LIBXMP}
)

if(GRRMOD_INSTALL)

  configure_file(grrmod-xmp.pc.in ${CMAKE_BINARY_DIR}/grrmod-xmp.pc @ONLY)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD/grrmod-xmp.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(FILES ${CMAKE_BINARY_DIR}/grrmod-xmp.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )

  install(
    TARGETS grrmod-xmp EXPORT grrmod-xmp-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

endif()
