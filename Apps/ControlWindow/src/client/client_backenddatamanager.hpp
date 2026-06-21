#pragma once

#include "client_base/httpclientbase.hpp"

class Client_BackendDataManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit Client_BackendDataManager(QObject *parent = nullptr);
};
