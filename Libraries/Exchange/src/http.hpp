#pragma once

#include <string>

namespace Exchange::HTTPv1 {

const std::string API_V {"/api/v1"};

// AI Manager server
const auto SERVER_BASE {API_V + "/manager"};
const auto SERVER_STATUS {SERVER_BASE + "/status"};

// Backends
const auto BACKENDS_GET {API_V + "/backends"};

const auto BACKEND_CONFIG_GET {API_V + "/backend/{uuid}"};
const auto BACKEND_CONFIG_SET {API_V + "/backend/{uuid}"};

const auto QT_BACKEND_CONFIG_GET {API_V + "/backend/%1"};
const auto QT_BACKEND_CONFIG_SET {API_V + "/backend/%1"};


// Developing
const auto DEVELOP_BASE     {API_V + "/dev"};
const auto DEVELOP_START    {DEVELOP_BASE + "?action=start"};
const auto DEVELOP_STATUS   {DEVELOP_BASE};
const auto DEVELOP_STOP     {DEVELOP_BASE + "?action=stop"};

} // namespace Exchange::HTTPv1
