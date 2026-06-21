#pragma once

#include <string>

namespace Exchange::HTTPv1 {

const std::string API_V {"/api/v1"};

// AI Manager server
const auto SERVER_BASE {API_V + "/manager"};
const auto SERVER_STATUS {SERVER_BASE + "/status"};

// Backends
const auto BACKENDS_GET {API_V + "/backends"};

const auto BACKEND_STATUS {API_V + "/backend/{backendId}/status"};

const auto BACKEND_CONFIG_GET {API_V + "/backend/{backendId}/config"};
const auto BACKEND_CONFIG_SET {API_V + "/backend/{backendId}/config"};

const auto QT_BACKEND_CONFIG_GET {API_V + "/backend/%1/config"};
const auto QT_BACKEND_CONFIG_SET {API_V + "/backend/%1/config"};


// User tasks processing
const auto USER_REQUEST_BASE     {API_V + "/task"}; // TODO: Process many tasks in future?
const auto USER_REQUEST_START    {USER_REQUEST_BASE + "?action=start"};
const auto USER_REQUEST_STATUS   {USER_REQUEST_BASE + "/status"};
const auto USER_REQUEST_STOP     {USER_REQUEST_BASE + "?action=stop"};

} // namespace Exchange::HTTPv1
