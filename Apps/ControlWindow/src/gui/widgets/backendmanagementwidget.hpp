#pragma once

#include <QWidget>

namespace Ui {
class BackendManagementWidget;
}

class BackendManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendManagementWidget(QWidget *parent = nullptr);
    ~BackendManagementWidget();

private:
    Ui::BackendManagementWidget *ui;
};
