# Install script for directory: /home/omar/ns-3-dev/src/core

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
    set(CMAKE_INSTALL_CONFIG_NAME "default")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so"
         RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/omar/ns-3-dev/build/lib/libns3.39-core-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so"
         OLD_RPATH "/home/omar/ns-3-dev/build/lib:::::::::"
         NEW_RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-core-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/omar/ns-3-dev/src/core/model/int64x64-128.h"
    "/home/omar/ns-3-dev/src/core/helper/csv-reader.h"
    "/home/omar/ns-3-dev/src/core/helper/event-garbage-collector.h"
    "/home/omar/ns-3-dev/src/core/helper/random-variable-stream-helper.h"
    "/home/omar/ns-3-dev/src/core/model/abort.h"
    "/home/omar/ns-3-dev/src/core/model/ascii-file.h"
    "/home/omar/ns-3-dev/src/core/model/ascii-test.h"
    "/home/omar/ns-3-dev/src/core/model/assert.h"
    "/home/omar/ns-3-dev/src/core/model/attribute-accessor-helper.h"
    "/home/omar/ns-3-dev/src/core/model/attribute-construction-list.h"
    "/home/omar/ns-3-dev/src/core/model/attribute-container.h"
    "/home/omar/ns-3-dev/src/core/model/attribute-helper.h"
    "/home/omar/ns-3-dev/src/core/model/attribute.h"
    "/home/omar/ns-3-dev/src/core/model/boolean.h"
    "/home/omar/ns-3-dev/src/core/model/breakpoint.h"
    "/home/omar/ns-3-dev/src/core/model/build-profile.h"
    "/home/omar/ns-3-dev/src/core/model/calendar-scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/callback.h"
    "/home/omar/ns-3-dev/src/core/model/command-line.h"
    "/home/omar/ns-3-dev/src/core/model/config.h"
    "/home/omar/ns-3-dev/src/core/model/default-deleter.h"
    "/home/omar/ns-3-dev/src/core/model/default-simulator-impl.h"
    "/home/omar/ns-3-dev/src/core/model/deprecated.h"
    "/home/omar/ns-3-dev/src/core/model/des-metrics.h"
    "/home/omar/ns-3-dev/src/core/model/double.h"
    "/home/omar/ns-3-dev/src/core/model/enum.h"
    "/home/omar/ns-3-dev/src/core/model/event-id.h"
    "/home/omar/ns-3-dev/src/core/model/event-impl.h"
    "/home/omar/ns-3-dev/src/core/model/fatal-error.h"
    "/home/omar/ns-3-dev/src/core/model/fatal-impl.h"
    "/home/omar/ns-3-dev/src/core/model/fd-reader.h"
    "/home/omar/ns-3-dev/src/core/model/environment-variable.h"
    "/home/omar/ns-3-dev/src/core/model/global-value.h"
    "/home/omar/ns-3-dev/src/core/model/hash-fnv.h"
    "/home/omar/ns-3-dev/src/core/model/hash-function.h"
    "/home/omar/ns-3-dev/src/core/model/hash-murmur3.h"
    "/home/omar/ns-3-dev/src/core/model/hash.h"
    "/home/omar/ns-3-dev/src/core/model/heap-scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/int-to-type.h"
    "/home/omar/ns-3-dev/src/core/model/int64x64-double.h"
    "/home/omar/ns-3-dev/src/core/model/int64x64.h"
    "/home/omar/ns-3-dev/src/core/model/integer.h"
    "/home/omar/ns-3-dev/src/core/model/length.h"
    "/home/omar/ns-3-dev/src/core/model/list-scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/log-macros-disabled.h"
    "/home/omar/ns-3-dev/src/core/model/log-macros-enabled.h"
    "/home/omar/ns-3-dev/src/core/model/log.h"
    "/home/omar/ns-3-dev/src/core/model/make-event.h"
    "/home/omar/ns-3-dev/src/core/model/map-scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/math.h"
    "/home/omar/ns-3-dev/src/core/model/names.h"
    "/home/omar/ns-3-dev/src/core/model/node-printer.h"
    "/home/omar/ns-3-dev/src/core/model/nstime.h"
    "/home/omar/ns-3-dev/src/core/model/object-base.h"
    "/home/omar/ns-3-dev/src/core/model/object-factory.h"
    "/home/omar/ns-3-dev/src/core/model/object-map.h"
    "/home/omar/ns-3-dev/src/core/model/object-ptr-container.h"
    "/home/omar/ns-3-dev/src/core/model/object-vector.h"
    "/home/omar/ns-3-dev/src/core/model/object.h"
    "/home/omar/ns-3-dev/src/core/model/pair.h"
    "/home/omar/ns-3-dev/src/core/model/pointer.h"
    "/home/omar/ns-3-dev/src/core/model/priority-queue-scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/ptr.h"
    "/home/omar/ns-3-dev/src/core/model/random-variable-stream.h"
    "/home/omar/ns-3-dev/src/core/model/rng-seed-manager.h"
    "/home/omar/ns-3-dev/src/core/model/rng-stream.h"
    "/home/omar/ns-3-dev/src/core/model/scheduler.h"
    "/home/omar/ns-3-dev/src/core/model/show-progress.h"
    "/home/omar/ns-3-dev/src/core/model/simple-ref-count.h"
    "/home/omar/ns-3-dev/src/core/model/simulation-singleton.h"
    "/home/omar/ns-3-dev/src/core/model/simulator-impl.h"
    "/home/omar/ns-3-dev/src/core/model/simulator.h"
    "/home/omar/ns-3-dev/src/core/model/singleton.h"
    "/home/omar/ns-3-dev/src/core/model/string.h"
    "/home/omar/ns-3-dev/src/core/model/synchronizer.h"
    "/home/omar/ns-3-dev/src/core/model/system-path.h"
    "/home/omar/ns-3-dev/src/core/model/system-wall-clock-ms.h"
    "/home/omar/ns-3-dev/src/core/model/system-wall-clock-timestamp.h"
    "/home/omar/ns-3-dev/src/core/model/test.h"
    "/home/omar/ns-3-dev/src/core/model/time-printer.h"
    "/home/omar/ns-3-dev/src/core/model/timer-impl.h"
    "/home/omar/ns-3-dev/src/core/model/timer.h"
    "/home/omar/ns-3-dev/src/core/model/trace-source-accessor.h"
    "/home/omar/ns-3-dev/src/core/model/traced-callback.h"
    "/home/omar/ns-3-dev/src/core/model/traced-value.h"
    "/home/omar/ns-3-dev/src/core/model/trickle-timer.h"
    "/home/omar/ns-3-dev/src/core/model/tuple.h"
    "/home/omar/ns-3-dev/src/core/model/type-id.h"
    "/home/omar/ns-3-dev/src/core/model/type-name.h"
    "/home/omar/ns-3-dev/src/core/model/type-traits.h"
    "/home/omar/ns-3-dev/src/core/model/uinteger.h"
    "/home/omar/ns-3-dev/src/core/model/unused.h"
    "/home/omar/ns-3-dev/src/core/model/valgrind.h"
    "/home/omar/ns-3-dev/src/core/model/vector.h"
    "/home/omar/ns-3-dev/src/core/model/warnings.h"
    "/home/omar/ns-3-dev/src/core/model/watchdog.h"
    "/home/omar/ns-3-dev/src/core/model/realtime-simulator-impl.h"
    "/home/omar/ns-3-dev/src/core/model/wall-clock-synchronizer.h"
    "/home/omar/ns-3-dev/src/core/model/val-array.h"
    "/home/omar/ns-3-dev/src/core/model/matrix-array.h"
    "/home/omar/ns-3-dev/build/include/ns3/config-store-config.h"
    "/home/omar/ns-3-dev/build/include/ns3/core-config.h"
    "/home/omar/ns-3-dev/build/include/ns3/core-module.h"
    )
endif()

