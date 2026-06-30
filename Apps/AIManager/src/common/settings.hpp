#pragma once

namespace Settings
{

// System settings
const auto SECTION_SYSTEM       { "AIMANAGER" };
const auto SYSTEM_MANAGER_TOKEN {"manager-token"};
const auto SYSTEM_API_PORT      {"control-api-port"};
const auto SYSTEM_INPUT_MODEL   {"input-model"};

// Database connection settings
const auto SECTION_DB   { "DATABASE" };
const auto DB_ADDRESS   {"address"};
const auto DB_PORT      {"port"};
const auto DB_DBNAME    {"db-name"};
const auto DB_USERNAME  {"username"};
const auto DB_USER_PASS {"password"};

}
