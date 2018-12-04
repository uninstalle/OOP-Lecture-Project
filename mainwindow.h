#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Histogram.h"

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
	QImage Image;
    void paintEvent(QPaintEvent *);
private:
    Ui::MainWindow *ui;
	Histogram *h;
};

#endif // MAINWINDOW_H
