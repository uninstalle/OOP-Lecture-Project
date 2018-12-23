#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include "ImageConverter.h"
#include "ImageProcess.h"

ImageProcess newProcessor;
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	auto image = new QImage("1.jpg");
	auto mat = ImageConverter::QImageToMat(*image);
	mat = ImageProcess::GaussianBlur(newProcessor, mat, 1);
	newProcessor.Layers.push_back(mat);

	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	for (auto layer : newProcessor.Layers)
		painter.drawImage(QRect{ 0,0,this->width(),this->height() }, ImageConverter::MatToQImage(layer));

}


