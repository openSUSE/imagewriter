cmake_minimum_required(VERSION 3.19)
project(Imagewriter VERSION 1.10 LANGUAGES CXX)

# try to find Qt6
find_package(Qt6 6.2
  COMPONENTS DBus Gui Widgets)
if (Qt6_FOUND)
  message(STATUS "Using Qt6")
  qt_standard_project_setup()
else()
  # try to fall back to Qt5
  find_package(Qt5 5.15
    COMPONENTS DBus Gui Widgets)
  if (Qt5_FOUND)
    message(STATUS "Using Qt5")
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
  else()
    message(FATAL_ERROR "Qt6 and Qt5 not found")
  endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility=hidden -fvisibility-inlines-hidden")

add_executable(imagewriter
  main.cpp
  MainWindow.cpp
  PlatformHal.cpp
  PlatformUdisks.cpp
  Platform.cpp
  PlatformUdisks2.cpp
  udisks2_interface.cpp
  udisks2_mountpoints_interface.cpp
  imagewriter.qrc
)

target_compile_definitions(imagewriter PUBLIC "APP_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
target_compile_definitions(imagewriter PUBLIC USEUDISKS2)

target_link_libraries(imagewriter
  PRIVATE Qt::DBus Qt::Gui Qt::Widgets)
