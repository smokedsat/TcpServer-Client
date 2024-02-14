cmake_minimum_required(VERSION 3.14)

project(QTCPServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Network Core)

add_executable(QTCPServer
  main.cpp
#  tcpserver.h tcpserver.cpp
)
target_link_libraries(QTCPServer Qt${QT_VERSION_MAJOR}::Core  Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS QTCPServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
