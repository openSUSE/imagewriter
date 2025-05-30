cmake_minimum_required(VERSION 3.19)
project(Imagewriter VERSION 1.10 LANGUAGES CXX)

find_package(Qt6 REQUIRED
  COMPONENTS DBus Gui Widgets)

qt_standard_project_setup(REQUIRES 6.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility=hidden -fvisibility-inlines-hidden")

qt_add_executable(imagewriter
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
  PRIVATE Qt6::DBus Qt6::Gui Qt6::Widgets)
