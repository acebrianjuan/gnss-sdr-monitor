set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
     message(FATAL_ERROR "Fatal error: Boost required.")
endif()

find_package(Protobuf REQUIRED)
if(${Protobuf_VERSION} VERSION_LESS "3.0.0")
     message(FATAL_ERROR "Fatal error: Protocol Buffers > v3.0.0 required.")
endif()
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/protobuf/gnss_synchro.proto)
set_property(SOURCE ${PROTO_SRCS} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_HDRS} PROPERTY SKIP_AUTOGEN ON)

protobuf_generate_cpp(PROTO_SRCS2 PROTO_HDRS2 ${CMAKE_SOURCE_DIR}/src/protobuf/monitor_pvt.proto)
set_property(SOURCE ${PROTO_SRCS2} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_HDRS2} PROPERTY SKIP_AUTOGEN ON)

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
    telecommand_widget.cpp
    telnet_manager.cpp
    altitude_widget.cpp
    dop_widget.cpp
    ${PROTO_SRCS}
    ${PROTO_SRCS2}
)

set(UI_SOURCES
    main_window.ui
    preferences_dialog.ui
    telecommand_widget.ui
)

set(RESOURCES
    resources.qrc
)

add_executable(${TARGET} ${SOURCES} ${UI_SOURCES} ${RESOURCES})

target_link_libraries(${TARGET} PUBLIC ${QT5_LIBRARIES} Boost::boost protobuf::libprotobuf)

install(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
