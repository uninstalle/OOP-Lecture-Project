#include "mainwindow1.h"
#include "ui_mainwindow1.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pa = this->palette();
    pa.setBrush(QPalette::Background, QBrush(Qt::gray));
    setPalette(pa);

    setWindowTitle("Title");
    resize(1200, 800);

//    //移动
    move(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}
