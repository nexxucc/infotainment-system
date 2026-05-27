set(APP_SOURCES
    main.cpp
    ui/MainWindow.cpp
    ui/DashboardWidget.cpp
    ui/MediaWidget.cpp
    ui/BluetoothWidget.cpp
    ui/NavigationWidget.cpp
    ui/TelemetryWidget.cpp
    ui/SettingsWidget.cpp
)

set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StateManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CommandServer.cpp
)

set(MODULE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/modules/MediaManager.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/BluetoothManager.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/NavigationManager.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/TelemetryManager.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/SettingsManager.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/NotificationManager.cpp
)

set(VALIDATION_SOURCES
    ${CMAKE_SOURCE_DIR}/src/validation/InputValidator.cpp
)

add_executable(infotainment-simulator
    ${APP_SOURCES}
    ${CORE_SOURCES}
    ${MODULE_SOURCES}
    ${VALIDATION_SOURCES}
)

target_include_directories(infotainment-simulator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/app
)

target_link_libraries(infotainment-simulator PRIVATE
    Qt6::Widgets
    Qt6::Network
    nlohmann_json::nlohmann_json
)
