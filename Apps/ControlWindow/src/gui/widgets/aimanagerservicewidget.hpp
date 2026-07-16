#pragma once

#include <QWidget>

namespace Ui {
class AIManagerServiceWidget;
}

class AIManagerServiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIManagerServiceWidget(QWidget *parent = nullptr);
    ~AIManagerServiceWidget();

private:
    Ui::AIManagerServiceWidget *ui;
};
