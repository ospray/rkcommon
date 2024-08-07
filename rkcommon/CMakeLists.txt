## Copyright 2009 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

if (RKCOMMON_TASKING_INTERNAL)
  set(EXTRA_TASKING_SOURCES
    tasking/detail/enkiTS/TaskScheduler.cpp
    tasking/detail/TaskSys.cpp
  )
endif()

add_library(${PROJECT_NAME}
  ${RKCOMMON_RESOURCE}

  common.cpp

  memory/malloc.cpp

  networking/DataStreaming.cpp
  networking/Fabric.cpp

  os/FileName.cpp
  os/library.cpp

  tasking/detail/tasking_system_init.cpp

  ${EXTRA_TASKING_SOURCES}

  utility/demangle.cpp
  utility/ParameterizedObject.cpp
  utility/PseudoURL.cpp
  utility/TimeStamp.cpp

  xml/XML.cpp

  tracing/Tracing.cpp
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    rkcommon_tasking
    ${CMAKE_DL_LIBS}
    $<${RKCOMMON_PLATFORM_WIN}:ws2_32>
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

if (RKCOMMON_ADDRSAN)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DRKCOMMON_ADDRSAN)
endif()

if (RKCOMMON_NO_SIMD)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DRKCOMMON_NO_SIMD)
endif()

if (RKCOMMON_WARN_AS_ERRORS)
  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /WX)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
  endif()
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

## Install library + targets ##################################################

set_target_properties(${PROJECT_NAME}
    PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME}
  EXPORT rkcommon_Exports
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    NAMELINK_SKIP
  # on Windows put the dlls into bin
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  # ... and the import lib into the devel package
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT rkcommon_Exports
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rkcommon-${PROJECT_VERSION}
  NAMESPACE rkcommon::
)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    NAMELINK_ONLY
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

## Install headers ############################################################

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN *.h
  PATTERN *.inl
  PATTERN *.hpp
  PATTERN *.ih
)
