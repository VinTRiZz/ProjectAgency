#pragma once

#include <QWidget>

#include <ProjectAgency/AIObjects/OllamaConfig.h>

namespace Ui {
class AgentConfigWidget;
}

class AgentConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AgentConfigWidget(QWidget *parent = nullptr);
    ~AgentConfigWidget();

    void setConfig(AIObjects::OllamaConfigPtr pConfig);
    AIObjects::OllamaConfigPtr getConfig() const;

private:
    Ui::AgentConfigWidget *ui;

    AIObjects::OllamaConfigPtr m_pConfig;
};
