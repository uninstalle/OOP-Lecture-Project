#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include "ImageConverter.h"
#include "ImageProcess.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//这是一个例程，展示了GUI端没有包含OpenCV头文件情况下的使用方法。
	//请注意image是本地变量，因此需要使用QImageCopyToMat进行深复制
	//否则该函数结束时，image的data即被析构。
	//为什么不试试把QImageCopyToMat改成QImageToMat会发生什么呢？
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


