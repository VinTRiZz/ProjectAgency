#pragma once

#include <QWidget>

namespace Ui {
class AIManagerServiceWidget;
}

class AIManagerContext;

class AIManagerServiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIManagerServiceWidget(QWidget *parent = nullptr);
    ~AIManagerServiceWidget();

    void setContext(AIManagerContext* pContext);
    AIManagerContext* getManagerContext();

private:
    Ui::AIManagerServiceWidget *ui;

    AIManagerContext* m_pManagerContext {nullptr};
};
