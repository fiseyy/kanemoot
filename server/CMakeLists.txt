cmake_minimum_required(VERSION 3.19)
project(kanemoot-server LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(kanemoot-server
    main.cpp
)

target_link_libraries(kanemoot-server
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS kanemoot-server
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
