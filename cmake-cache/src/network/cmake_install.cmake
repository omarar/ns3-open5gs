# Install script for directory: /home/omar/ns-3-dev/src/network

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so"
         RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/omar/ns-3-dev/build/lib/libns3.39-network-default.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so"
         OLD_RPATH "/home/omar/ns-3-dev/build/lib:::::::::"
         NEW_RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3.39-network-default.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/omar/ns-3-dev/src/network/helper/application-container.h"
    "/home/omar/ns-3-dev/src/network/helper/delay-jitter-estimation.h"
    "/home/omar/ns-3-dev/src/network/helper/net-device-container.h"
    "/home/omar/ns-3-dev/src/network/helper/node-container.h"
    "/home/omar/ns-3-dev/src/network/helper/packet-socket-helper.h"
    "/home/omar/ns-3-dev/src/network/helper/simple-net-device-helper.h"
    "/home/omar/ns-3-dev/src/network/helper/trace-helper.h"
    "/home/omar/ns-3-dev/src/network/model/address.h"
    "/home/omar/ns-3-dev/src/network/model/application.h"
    "/home/omar/ns-3-dev/src/network/model/buffer.h"
    "/home/omar/ns-3-dev/src/network/model/byte-tag-list.h"
    "/home/omar/ns-3-dev/src/network/model/channel-list.h"
    "/home/omar/ns-3-dev/src/network/model/channel.h"
    "/home/omar/ns-3-dev/src/network/model/chunk.h"
    "/home/omar/ns-3-dev/src/network/model/header.h"
    "/home/omar/ns-3-dev/src/network/model/net-device.h"
    "/home/omar/ns-3-dev/src/network/model/nix-vector.h"
    "/home/omar/ns-3-dev/src/network/model/node-list.h"
    "/home/omar/ns-3-dev/src/network/model/node.h"
    "/home/omar/ns-3-dev/src/network/model/packet-metadata.h"
    "/home/omar/ns-3-dev/src/network/model/packet-tag-list.h"
    "/home/omar/ns-3-dev/src/network/model/packet.h"
    "/home/omar/ns-3-dev/src/network/model/socket-factory.h"
    "/home/omar/ns-3-dev/src/network/model/socket.h"
    "/home/omar/ns-3-dev/src/network/model/tag-buffer.h"
    "/home/omar/ns-3-dev/src/network/model/tag.h"
    "/home/omar/ns-3-dev/src/network/model/trailer.h"
    "/home/omar/ns-3-dev/src/network/test/header-serialization-test.h"
    "/home/omar/ns-3-dev/src/network/utils/address-utils.h"
    "/home/omar/ns-3-dev/src/network/utils/bit-deserializer.h"
    "/home/omar/ns-3-dev/src/network/utils/bit-serializer.h"
    "/home/omar/ns-3-dev/src/network/utils/crc32.h"
    "/home/omar/ns-3-dev/src/network/utils/data-rate.h"
    "/home/omar/ns-3-dev/src/network/utils/drop-tail-queue.h"
    "/home/omar/ns-3-dev/src/network/utils/dynamic-queue-limits.h"
    "/home/omar/ns-3-dev/src/network/utils/error-channel.h"
    "/home/omar/ns-3-dev/src/network/utils/error-model.h"
    "/home/omar/ns-3-dev/src/network/utils/ethernet-header.h"
    "/home/omar/ns-3-dev/src/network/utils/ethernet-trailer.h"
    "/home/omar/ns-3-dev/src/network/utils/flow-id-tag.h"
    "/home/omar/ns-3-dev/src/network/utils/generic-phy.h"
    "/home/omar/ns-3-dev/src/network/utils/inet-socket-address.h"
    "/home/omar/ns-3-dev/src/network/utils/inet6-socket-address.h"
    "/home/omar/ns-3-dev/src/network/utils/ipv4-address.h"
    "/home/omar/ns-3-dev/src/network/utils/ipv6-address.h"
    "/home/omar/ns-3-dev/src/network/utils/llc-snap-header.h"
    "/home/omar/ns-3-dev/src/network/utils/lollipop-counter.h"
    "/home/omar/ns-3-dev/src/network/utils/mac16-address.h"
    "/home/omar/ns-3-dev/src/network/utils/mac48-address.h"
    "/home/omar/ns-3-dev/src/network/utils/mac64-address.h"
    "/home/omar/ns-3-dev/src/network/utils/mac8-address.h"
    "/home/omar/ns-3-dev/src/network/utils/net-device-queue-interface.h"
    "/home/omar/ns-3-dev/src/network/utils/output-stream-wrapper.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-burst.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-data-calculators.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-probe.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-socket-address.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-socket-client.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-socket-factory.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-socket-server.h"
    "/home/omar/ns-3-dev/src/network/utils/packet-socket.h"
    "/home/omar/ns-3-dev/src/network/utils/packetbb.h"
    "/home/omar/ns-3-dev/src/network/utils/pcap-file-wrapper.h"
    "/home/omar/ns-3-dev/src/network/utils/pcap-file.h"
    "/home/omar/ns-3-dev/src/network/utils/pcap-test.h"
    "/home/omar/ns-3-dev/src/network/utils/queue-fwd.h"
    "/home/omar/ns-3-dev/src/network/utils/queue-item.h"
    "/home/omar/ns-3-dev/src/network/utils/queue-limits.h"
    "/home/omar/ns-3-dev/src/network/utils/queue-size.h"
    "/home/omar/ns-3-dev/src/network/utils/queue.h"
    "/home/omar/ns-3-dev/src/network/utils/radiotap-header.h"
    "/home/omar/ns-3-dev/src/network/utils/sequence-number.h"
    "/home/omar/ns-3-dev/src/network/utils/simple-channel.h"
    "/home/omar/ns-3-dev/src/network/utils/simple-net-device.h"
    "/home/omar/ns-3-dev/src/network/utils/sll-header.h"
    "/home/omar/ns-3-dev/src/network/utils/timestamp-tag.h"
    "/home/omar/ns-3-dev/build/include/ns3/network-module.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/omar/ns-3-dev/cmake-cache/src/network/examples/cmake_install.cmake")

endif()

