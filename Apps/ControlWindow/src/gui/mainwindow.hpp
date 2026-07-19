#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class AIManagerContext;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    AIManagerContext* m_pManagerContext;
};

