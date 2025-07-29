cmake_minimum_required(VERSION 3.16)

project(kanemoot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
    WebSockets
    Network
)
find_package(Qt6 REQUIRED COMPONENTS Core)


qt_add_executable(appkanemoot
    main.cpp
)

qt_add_qml_module(appkanemoot
    URI kanemoot
    VERSION 1.0
    QML_FILES
    SOURCES controller/appcontroller.h controller/appcontroller.cpp
    SOURCES controller/basepage.h
    SOURCES controller/loginpage.h controller/loginpage.cpp
    SOURCES controller/registrationpage.h controller/registrationpage.cpp
    SOURCES utils/logging.cpp utils/logging.h
    SOURCES core/app.h core/app.cpp
    QML_FILES ui/App.qml
    QML_FILES ui/pages/LoginPage.qml
    QML_FILES ui/components/AnimatedBackground.qml
    RESOURCES
    RESOURCES resources.qrc
    QML_FILES
    QML_FILES ui/components/ErrorToast.qml
    QML_FILES
    QML_FILES ui/components/ErrorToastManager.qml
    SOURCES core/errorhandler.h core/errorhandler.cpp
    QML_FILES ui/pages/RegistrationPage.qml
    SOURCES utils/authvalidator.h utils/authvalidator.cpp
    SOURCES network/websocketclient.h network/websocketclient.cpp

)

qt6_add_resources(RESOURCE_ADDED resources.qrc)
target_sources(appkanemoot PRIVATE ${RESOURCE_ADDED})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appkanemoot PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appkanemoot
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appkanemoot
    PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::QuickControls2
    Qt6::WebSockets
    Qt6::Network
)
target_link_libraries(appkanemoot PRIVATE Qt6::Core)
target_link_libraries(appkanemoot PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appkanemoot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
