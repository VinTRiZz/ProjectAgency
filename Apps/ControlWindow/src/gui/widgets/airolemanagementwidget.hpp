#pragma once

#include <QWidget>

namespace Ui {
class AIRoleManagementWidget;
}

class AIRoleManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIRoleManagementWidget(QWidget *parent = nullptr);
    ~AIRoleManagementWidget();

private:
    Ui::AIRoleManagementWidget *ui;
};
