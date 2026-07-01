if (NOT PAG_BUILD_CWINDOW)
    message(STATUS "Control window build disabled")
    return()
endif()

COMPONENTS_CCR_ADD_QT(5 Core Widgets Network)

# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_TARGET_NAME PAG-ControlWindow)

COMPONENTS_CCR_CONFIGURE_APP(${APP_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    Qt5::Widgets
    Qt5::Network
)

target_sources(PAG-ControlWindow
  PRIVATE
    src/client/client_backenddatamanager.hpp src/client/client_backenddatamanager.cpp
)

COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Common)

set(PAG_LIBRARIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries)
COMPONENTS_CCR_LINK_LIBRARY(${APP_TARGET_NAME} AIObjects ${PAG_LIBRARIES_ROOT}/AIObjects)
COMPONENTS_CCR_LINK_LIBRARY(${APP_TARGET_NAME} Exchange ${PAG_LIBRARIES_ROOT}/Exchange)