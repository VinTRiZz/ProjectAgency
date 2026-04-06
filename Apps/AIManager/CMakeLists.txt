if (NOT PAG_BUILD_MANAGER)
    message(STATUS "AI manager build disabled")
    return()
endif()

set(APP_TARGET_NAME PAG-AIManager)

COMPONENTS_CCR_CONFIGURE_APP(${APP_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    ssl
    crypto
    drogon
    boost_system
    boost_program_options
)


COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Common)
COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Encryption)
COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} Filework)
COMPONENTS_LINK_COMPONENT(${APP_TARGET_NAME} SystemProcessing)

if (websocketpp_SOURCE_DIR)
    target_include_directories(${APP_TARGET_NAME} PRIVATE ${websocketpp_SOURCE_DIR})
endif()

set(PAG_LIBRARIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries)
COMPONENTS_CCR_LINK_LIBRARY(${APP_TARGET_NAME} DataObjects ${PAG_LIBRARIES_ROOT}/DataObjects)