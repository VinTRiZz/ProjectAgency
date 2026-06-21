#pragma once

#include <QWidget>

namespace Ui {
class BackendStatusWidget;
}

class BackendStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendStatusWidget(QWidget *parent = nullptr);
    ~BackendStatusWidget();

private:
    Ui::BackendStatusWidget *ui;
};
