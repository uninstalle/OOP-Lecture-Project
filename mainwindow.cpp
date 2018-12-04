#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include "ImageConverter.h"
#include "ImageProcess.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	auto image = QImage("2chtex.png");
	Image = ImageConverter::MatToQImage(
		ImageProcess::generate3ChannelsNormalTexture(
			ImageConverter::QImageCopyToMat(image)));
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawImage(QRect{0,0,this->width(),this->height()},Image);

}


