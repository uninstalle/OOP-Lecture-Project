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
	auto image3 = new QImage("png.png");
	newProcessor.Layers.addLayerAsTop(ImageConverter::QImageToMat(*image));
	newProcessor.Layers.addLayerAsTop(ImageConverter::QImageToMat(*image2));
	newProcessor.Layers.addLayerAsTop(ImageConverter::QImageToMat(*image3));
	//newProcessor.deleteLayer(newProcessor.Layers.at(1));
	//ImageProcess::Sculpture(newProcessor, newProcessor.Layers.front());
	//newProcessor.Layers.mergeLayers(newProcessor.Layers[0], newProcessor.Layers[1], 1);
	newProcessor.Layers.mergeLayers(newProcessor.Layers[0], newProcessor.Layers[1], 0.1);
	//ImageProcess::AdjustContrastAndBrightness(newProcessor, newProcessor.Layers.front(), 0.5, 0);
	//ImageProcess::GaussianBlur(newProcessor, newProcessor.Layers.front(), 1.0);
	//newProcessor.revertChange();

	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	for (auto it = newProcessor.Layers.rbegin(); it != newProcessor.Layers.rend(); ++it)
	{
		QPoint topLeft(it->getTopLeftPoint().first, it->getTopLeftPoint().second);
		QPoint bottomRight(it->getBottomRightPoint().first, it->getBottomRightPoint().second);
		if (bottomRight.x() == 0 && bottomRight.y() == 0)
		{
			bottomRight.setX(this->width());
			bottomRight.setY(this->height());
		}
		QRect drawArea(topLeft, bottomRight);
		painter.drawImage(drawArea, ImageConverter::MatToQImage(it->getMat()));
	}

}