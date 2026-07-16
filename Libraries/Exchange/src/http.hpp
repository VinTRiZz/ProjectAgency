#pragma once

#include <string>

namespace Exchange::HTTPv1 {

const std::string API_V {"/api/v1"};

// AI Manager server
const auto SERVER_BASE      {API_V + "/manager"};
const auto SERVER_STATUS    {SERVER_BASE + "/status"};
const auto SERVER_ACTION    {SERVER_BASE + "?action={action}"};
const auto QT_SERVER_ACTION {SERVER_BASE + "?action=%1"};

/**
 * @brief The ServerAction enum Actions to work with AIBackend
 */
enum ServerAction : short
{
    ActionStop = 0,
    ActionRestart,
};

// All backend id list
const auto BACKENDS_ID_LIST {API_V + "/backends"};

// Target backend
const auto BACKEND_BASE         {API_V + "/backend"};
const auto BACKEND_STATUS       {BACKEND_BASE + "/{backendId}/status"};
const auto BACKEND_RECONNECT    {BACKEND_BASE + "/{backendId}/reconnect"};

// CRUD of backend information
const auto BACKEND_CONFIG_ADD {BACKEND_BASE + "?action=create"};
const auto BACKEND_CONFIG_GET {BACKEND_BASE + "/{backendId}/config"};
const auto BACKEND_CONFIG_SET {BACKEND_BASE + "/{backendId}/config"};
const auto BACKEND_CONFIG_REM {BACKEND_BASE + "/{backendId}/config"};

// CRUD of backend information
const auto QT_BACKEND_CONFIG_ADD {BACKEND_BASE + "?action=create"};
const auto QT_BACKEND_CONFIG_GET {BACKEND_BASE + "/%1/config"};
const auto QT_BACKEND_CONFIG_SET {BACKEND_BASE + "/%1/config"};
const auto QT_BACKEND_CONFIG_REM {BACKEND_BASE + "/%1/config"};


// User tasks processing
const auto USER_REQUEST_BASE     {API_V + "/task"}; // TODO: Process many tasks in future?
const auto USER_REQUEST_START    {USER_REQUEST_BASE + "?action=start"};
const auto USER_REQUEST_STATUS   {USER_REQUEST_BASE + "/status"};
const auto USER_REQUEST_STOP     {USER_REQUEST_BASE + "?action=stop"};

} // namespace Exchange::HTTPv1
