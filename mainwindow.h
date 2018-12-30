#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QtWidgets/QMainWindow>

class QPainter;
class QImage;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
