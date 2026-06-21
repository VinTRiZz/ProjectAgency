#pragma once

#include <QWidget>

#include <ProjectAgency/OllamaConfig.h>

namespace Ui {
class AgentConfigWidget;
}

class AgentConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AgentConfigWidget(QWidget *parent = nullptr);
    ~AgentConfigWidget();

    void setConfig(DataObjects::OllamaConfigPtr pConfig);
    DataObjects::OllamaConfigPtr getConfig() const;

private:
    Ui::AgentConfigWidget *ui;

    DataObjects::OllamaConfigPtr m_pConfig;
};
