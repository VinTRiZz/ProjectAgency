#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace Exchange {
class Error;
}

class AIManagerContext;
class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slot_processError(const Exchange::Error& err);

private:
    Ui::MainWindow *ui;

    AIManagerContext* m_pManagerContext;
    QMessageBox* m_pErrorMessageBox {nullptr};
};

