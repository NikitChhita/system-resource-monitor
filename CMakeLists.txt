cmake_minimum_required(VERSION 3.19)
project(untitled LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

qt_add_executable(Real-Time-System-Monitor
    WIN32 MACOSX_BUNDLE
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CpuMonitorUsage.h
    CpuMonitorUsage.cpp
    RamUsage.h
    RamUsage.cpp
    Network.h
    Network.cpp
    DiskInfo.h
    DiskInfo.cpp
    ProcessInfo.h
    ProcessInfo.cpp
    UsageGraph.h
    UsageGraph.cpp
    PageCustomization.h
    PageCustomization.cpp
)

target_link_libraries(Real-Time-System-Monitor
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Network
)

include(GNUInstallDirs)

install(TARGETS Real-Time-System-Monitor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Real-Time-System-Monitor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
