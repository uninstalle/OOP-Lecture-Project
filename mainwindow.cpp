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
	auto image2 = new QImage("2.jpg");
	auto image3 = new QImage("3.jpg");
	newProcessor.loadImageAsTopLayer(ImageConverter::QImageToMat(*image));
	newProcessor.loadImageAsBottomLayer(ImageConverter::QImageToMat(*image2));
	newProcessor.loadImageAsBottomLayer(ImageConverter::QImageToMat(*image3));
	newProcessor.moveLayerUp(newProcessor.Layers.at(1));
	newProcessor.deleteLayer(newProcessor.Layers.at(2));
	ImageProcess::Sculpture(newProcessor, newProcessor.Layers.back());
	//newProcessor.revertChange();
	//ImageProcess::GaussianBlur(newProcessor, newProcessor.Layers.front(), 1.0);

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
	{
		QPoint topLeft(layer.getLeftUpPoint().first, layer.getLeftUpPoint().second);
		QPoint bottomRight(layer.getRightDownPoint().first, layer.getRightDownPoint().second);
		if (bottomRight.x() == 0 && bottomRight.y() == 0)
		{
			bottomRight.setX(this->width());
			bottomRight.setY(this->height());
		}
		QRect drawArea(topLeft,bottomRight);
		painter.drawImage(drawArea, ImageConverter::MatToQImage(layer.getMat()));
	}

}


