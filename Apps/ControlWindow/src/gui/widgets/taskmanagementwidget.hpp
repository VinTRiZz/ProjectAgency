#pragma once

#include <QWidget>

namespace Ui {
class TaskManagementWidget;
}

class TaskManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskManagementWidget(QWidget *parent = nullptr);
    ~TaskManagementWidget();

private:
    Ui::TaskManagementWidget *ui;
};
