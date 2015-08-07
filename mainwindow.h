#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/core.hpp>

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();

    void on_actionFloodFill_triggered();

    void on_actionDetect_face_triggered();

    void on_actionClose_operation_triggered();

    void on_action_Save_triggered();

    void on_actionBatch_Processing_triggered();

private:
    Ui::MainWindow *ui;
    QString lastPath;
};

#endif // MAINWINDOW_H
