set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StateManager.cpp
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

set(TEST_SOURCES
    test_event_bus.cpp
    test_state_manager.cpp
    test_media_manager.cpp
    test_bluetooth_manager.cpp
    test_input_validator.cpp
)

add_executable(infotainment-tests
    ${TEST_SOURCES}
    ${CORE_SOURCES}
    ${MODULE_SOURCES}
    ${VALIDATION_SOURCES}
)

target_include_directories(infotainment-tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(infotainment-tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Network
    nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(infotainment-tests)
