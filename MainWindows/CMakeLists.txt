cmake_minimum_required(VERSION 3.19)
project(MainWindows LANGUAGES CXX)
set(CMAKE_AUTORCC ON)
# Find Qt6 and all required modules in one single line
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(MainWindows
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindows.h
    mainwindows.cpp
    resources.qrc
)

# Link all the Qt6 modules to your application in one single line
target_link_libraries(MainWindows PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)


include(GNUInstallDirs)

install(TARGETS MainWindows
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#qt_generate_deploy_app_script(
 #   TARGET MainWindows
 #   OUTPUT_SCRIPT deploy_script
  #  NO_UNSUPPORTED_PLATFORM_ERROR
#)
#install(SCRIPT ${deploy_script})
