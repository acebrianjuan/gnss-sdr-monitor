set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS system serialization REQUIRED)
if(NOT Boost_FOUND)
     message(FATAL_ERROR "Fatal error: Boost required.")
endif(NOT Boost_FOUND)

find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport Quick QuickWidgets Positioning Charts REQUIRED)
if(NOT Qt5_FOUND)
     message(FATAL_ERROR "Fatal error: Qt5 required.")
endif(NOT Qt5_FOUND)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT5_LIBRARIES
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::PrintSupport
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::Positioning
    Qt5::Charts
)

set(TARGET ${CMAKE_PROJECT_NAME})

set(SOURCES
    channel_table_model.cpp
    cn0_delegate.cpp
    constellation_delegate.cpp
    doppler_delegate.cpp
    led_delegate.cpp
    main.cpp
    main_window.cpp
    monitor_pvt_wrapper.cpp
    preferences_dialog.cpp
)

set(UI_SOURCES
    main_window.ui
    preferences_dialog.ui
)

set(RESOURCES
    resources.qrc
)

add_executable(${TARGET} ${SOURCES} ${UI_SOURCES} ${RESOURCES})

target_link_libraries(${TARGET} ${QT5_LIBRARIES} ${Boost_LIBRARIES})
