#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QString>
#include <string>
#include <QPainter>
#include <vector>
#include <QFileDialog>
#include <qmessagebox.h>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include <QMouseEvent>
#include <QSlider>
#include <QMessageBox>
#include <QDebug>
#include <ImageProcess.h>
#include <process_exception.h>
#include <ImageConverter.h>

ImageConverter converter;
ImageProcess newProcessor;
int mouseState=0;
int priX = 0;
int priY = 0;
unsigned nowid = 0;
using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBox->removeItem(0);
    ui->toolBox->removeItem(0);
//    ui->label_Draw->setAlignment(Qt::AlignCenter);
    ui->slider_r->setMinimum(0);
    ui->slider_r->setMaximum(255);
    ui->slider_g->setMinimum(0);
    ui->slider_g->setMaximum(255);
    ui->slider_b->setMinimum(0);
    ui->slider_b->setMaximum(255);
	ui->slider_region->setMinimum(0);
	ui->slider_region->setMaximum(20);
	ui->slider_angle->setMinimum(0);
	ui->slider_angle->setMaximum(359);
	ui->spinBox_angle->setRange(0, 359);
    ui->horizontalSlider_2->setMinimum(0);
    ui->horizontalSlider_2->setMaximum(10);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerUpDate()));
	timer->start(50);
	
}
void MainWindow::on_save_clicked() {
	try {

		auto bg = newProcessor.Layers.rbegin();
		for (auto it = newProcessor.Layers.rbegin(); it != newProcessor.Layers.rend(); ++it)
		{
			if (it == bg) continue;
			newProcessor.Layers.mergeLayers(*bg, *it);
		}

		QImage* savedimg = new QImage(ImageConverter::MatToQImage(newProcessor.Layers.rbegin()->getMat()));
		QFileDialog m_QFileDialog;
		QFileDialog *fileDialog = new QFileDialog(this);
		fileDialog->setWindowTitle(tr("Save"));
		fileDialog->setDirectory(".");
		QStringList fileNames;
		if (fileDialog->exec()) {
			fileNames = fileDialog->selectedFiles();
		}
		if (fileNames.length() != 0) {

			savedimg->save(fileNames[0] + ".jpg", "JPG", -1);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning","CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerUpDate() {
	update();
}

void MainWindow::on_actionOpen_triggered() {
	try {
		QFileDialog *fileDialog = new QFileDialog(this);
		fileDialog->setWindowTitle(tr("Open"));
		fileDialog->setDirectory(".");
		fileDialog->setNameFilter(tr("Images(*.jpg)"));
		QStringList fileNames;
		if (fileDialog->exec()) {
			fileNames = fileDialog->selectedFiles();
		}
		if (fileNames.length() != 0) {

			QImage* temp = new QImage(fileNames[0]);


			newProcessor.Layers.addLayerAsBottom(ImageConverter::QImageToMat(*temp));
			nowid++;
			QString title;
			title.setNum(nowid);//int to QString
			QByteArray ba = title.toLatin1();
			const char* ch = ba.data();//QString to char*
			ui->toolBox->addItem(new QLabel(ch), tr(ch));
			newProcessor.Layers.findLayer(nowid).enableProperty(0x1);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_actionSave_triggered() {
	auto bg = newProcessor.Layers.rbegin();
	try {
		for (auto it = newProcessor.Layers.rbegin(); it != newProcessor.Layers.rend(); ++it)
		{
			if (it == bg) continue;
			newProcessor.Layers.mergeLayers(*bg, *it);
		}

		QImage* savedimg = new QImage(ImageConverter::MatToQImage(newProcessor.Layers.rbegin()->getMat()));
		QFileDialog m_QFileDialog;
		QFileDialog *fileDialog = new QFileDialog(this);
		fileDialog->setWindowTitle(tr("Save"));
		fileDialog->setDirectory(".");
		QStringList fileNames;
		if (fileDialog->exec()) {
			fileNames = fileDialog->selectedFiles();
		}
		if (fileNames.length() != 0) {

			savedimg->save(fileNames[0] + ".jpg", "JPG", -1);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::action_open() {
	try {
		QFileDialog *fileDialog = new QFileDialog(this);
		fileDialog->setWindowTitle(tr("Open"));
		fileDialog->setDirectory(".");
		fileDialog->setNameFilter(tr("Images(*.jpg)"));
		QStringList fileNames;
		if (fileDialog->exec()) {
			fileNames = fileDialog->selectedFiles();
		}
		if (fileNames.length() != 0) {

			QImage* temp = new QImage(fileNames[0]);


			newProcessor.Layers.addLayerAsBottom(ImageConverter::QImageToMat(*temp));
			nowid++;
			QString title;
			title.setNum(nowid);//int to QString
			QByteArray ba = title.toLatin1();
			const char* ch = ba.data();//QString to char*
			ui->toolBox->addItem(new QLabel(ch), tr(ch));
			newProcessor.Layers.findLayer(nowid).enableProperty(0x1);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_addLayer_clicked() {
	try {

		QFileDialog *fileDialog = new QFileDialog(this);
		fileDialog->setWindowTitle(tr("Open"));
		fileDialog->setDirectory(".");
		fileDialog->setNameFilter(tr("Images(*.jpg)"));
		QStringList fileNames;
		if (fileDialog->exec()) {
			fileNames = fileDialog->selectedFiles();
		}
		if (fileNames.length() != 0) {

			QImage* temp = new QImage(fileNames[0]);


			newProcessor.Layers.addLayerAsBottom(ImageConverter::QImageToMat(*temp));
			nowid++;
			QString title;
			title.setNum(nowid);//int to QString
			QByteArray ba = title.toLatin1();
			const char* ch = ba.data();//QString to char*
			ui->toolBox->addItem(new QLabel(ch), tr(ch));
			newProcessor.Layers.findLayer(nowid).enableProperty(0x1);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_layerUp_clicked() {
	try {
		int index = ui->toolBox->currentIndex();
		unsigned id = ui->toolBox->itemText(index).toInt();
		if (index != 0) {
			QString ch(ui->toolBox->itemText(index));
			newProcessor.Layers.moveLayerUpByID(id);
			ui->toolBox->removeItem(index);
			ui->toolBox->insertItem(index - 1, new QLabel(ch), ch);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_layerDown_clicked() {
	int index = ui->toolBox->currentIndex();
	unsigned id = ui->toolBox->itemText(index).toInt();
	if (index != ui->toolBox->count() - 1) {
		QString ch(ui->toolBox->itemText(index));
		newProcessor.Layers.moveLayerUpByID(id);
		ui->toolBox->removeItem(index);
		ui->toolBox->insertItem(index + 1, new QLabel(ch), ch);
	}

}

void MainWindow::mouseReleaseEvent(QMouseEvent* e) {
	try {
		if (mouseState == 2) {
			int index = ui->pribox->currentIndex();
			ElementType t;
			switch (index)
			{
			case 0:t = ELLIPSE; break;
			case 1:t = LINE1; break;
			case 2:t = LINE2; break;
			case 3:t = TRIANGLE; break;
			case 4:t = RECTANGULAR; break;
			default:break;
			}
			if (e->x() - priX >= 0 && e->y() - priY >= 0) {
				Layer* temp = new Layer(t, priX, priY, e->x(), e->y());
				newProcessor.Layers.addLayerAsTop(*temp);

				nowid++;
				QString title;
				title.setNum(nowid);//int to QString
				QByteArray ba = title.toLatin1();
				const char* ch = ba.data();//QString to char*
				ui->toolBox->insertItem(0, new QLabel(ch), tr(ch));
				newProcessor.Layers.findLayer(nowid).enableProperty(0x1);

				mouseState = 0;
			}

		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_merge_clicked() {
	try {
		auto bg = newProcessor.Layers.rbegin();
		for (auto it = newProcessor.Layers.rbegin(); it != newProcessor.Layers.rend(); ++it)
		{
			if (it == bg) continue;
			newProcessor.Layers.mergeLayers(*it, *bg);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::mousePressEvent(QMouseEvent* e) {
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		if (mouseState == 1 && e->button() == Qt::LeftButton
			&&e->x() <= newProcessor.Layers.findLayer(ID).getBottomRightPoint().first && e->y() <= newProcessor.Layers.findLayer(ID).getBottomRightPoint().second
			&&e->x() >= newProcessor.Layers.findLayer(ID).getTopLeftPoint().first && e->y() >= newProcessor.Layers.findLayer(ID).getTopLeftPoint().second) {
			try {
				newProcessor.MagicWand(newProcessor, newProcessor.Layers.findLayer(ID), e->x(), e->y(), ui->slider_region->value(), ui->slider_r->value(), ui->slider_g->value(), ui->slider_b->value());
			}
			catch (...) {
				QMessageBox::warning(NULL, "warning", "PRIMITIVE and TEXT cannot use magicwand.", QMessageBox::Yes, QMessageBox::Yes);
			}

		}
		else if (mouseState == 2 && e->button() == Qt::LeftButton) {
			priX = e->x();
			priY = e->y();

		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}


void MainWindow::paintEvent(QPaintEvent *e)
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
		if (it->checkProperty(0x1)) {
			painter.drawImage(drawArea, ImageConverter::MatToQImage(it->getMat()));
		}
	}
	
	
}

void MainWindow::on_deleteLayer_clicked()
{
    //  ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
    int index = ui->toolBox->currentIndex();
    QString dt = ui->toolBox->itemText(index);
    if (index != -1)
    {
        newProcessor.Layers.deleteLayer(dt.toInt());
        ui->toolBox->removeItem(index);
    }
}

void MainWindow::on_NostalgicHue_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.NostalgicHue(newProcessor, newProcessor.Layers.findLayer(ID));
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_StrongLight_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.StrongLight(newProcessor, newProcessor.Layers.findLayer(ID));
		update();
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}


void MainWindow::on_DarkTown_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.DarkTown(newProcessor, newProcessor.Layers.findLayer(ID), ui->spinBox_2->value());
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_Feather_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.Feather(newProcessor, newProcessor.Layers.findLayer(ID), ui->spinBox_2->value());
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_Mosaic_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.Mosaic(newProcessor, newProcessor.Layers.findLayer(ID), ui->spinBox_2->value());
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_Sculpture_triggered()
{
	try {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.Sculpture(newProcessor, newProcessor.Layers.findLayer(ID));
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}


void MainWindow::on_Diffusion_triggered()
{
    int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
    newProcessor.Diffusion(newProcessor, newProcessor.Layers.findLayer(ID));
}

void MainWindow::on_GaussianBlur_triggered()
{
    int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
    newProcessor.GaussianBlur(newProcessor,newProcessor.Layers.findLayer(ID) ,ui->spinBox_2->value());
}

void MainWindow::on_Wind_triggered()
{
    int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
    newProcessor.Wind(newProcessor,newProcessor.Layers.findLayer(ID) ,ui->spinBox_2->value());
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    ui->spinBox_2->setValue(value);
}



void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->horizontalSlider_2->setValue(arg1);
}

void MainWindow::on_slider_angle_valueChanged(int value)
{
	static int last_angle = 0;
	ui->spinBox_angle->setValue(value);
	if (ui->toolBox->count() != 0) {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.ImgRotate(newProcessor, newProcessor.Layers.findLayer(ID), value-last_angle);
		last_angle = value;
	}
}

void MainWindow::on_spinBox_angle_valueChanged(int arg1)
{
	ui->slider_angle->setValue(arg1);
}

void MainWindow::on_flipHor_clicked() {
	if (ui->toolBox->count() != 0) {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.ImgFlip(newProcessor, newProcessor.Layers.findLayer(ID),1);
	}
}

void MainWindow::on_actionBack_triggered() {
	newProcessor.revertChange();
}

void MainWindow::on_flipVer_clicked() {
	if (ui->toolBox->count() != 0) {
		int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
		newProcessor.ImgFlip(newProcessor, newProcessor.Layers.findLayer(ID), 0);
	}
}

void MainWindow::on_magicWand_btn_clicked() {
	mouseState = 1;
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	try {
		if (ui->toolBox->count() != 0) {
			int ID = ui->toolBox->itemText(ui->toolBox->currentIndex()).toInt();
			if (newProcessor.Layers.findLayer(ID).checkProperty(0x8)) {
				switch (e->key()) {
				case Qt::Key_W:
				case Qt::Key_Up:
					newProcessor.moveText(newProcessor, newProcessor.Layers.findLayer(ID), 0, -10);
					break;
				case Qt::Key_S:
				case Qt::Key_Down:
					newProcessor.moveText(newProcessor, newProcessor.Layers.findLayer(ID), 0, 10);
					break;
				case Qt::Key_A:
				case Qt::Key_Left:
					newProcessor.moveText(newProcessor, newProcessor.Layers.findLayer(ID), -10, 0);
					break;
				case Qt::Key_D:
				case Qt::Key_Right:
					newProcessor.moveText(newProcessor, newProcessor.Layers.findLayer(ID), 10, 0);
					break;

				}
			}
			else if (newProcessor.Layers.findLayer(ID).checkProperty(0x4)) {
				switch (e->key()) {
				case Qt::Key_W:
				case Qt::Key_Up:
					newProcessor.movePrimitive(newProcessor, newProcessor.Layers.findLayer(ID), 0, -10);
					break;
				case Qt::Key_S:
				case Qt::Key_Down:
					newProcessor.movePrimitive(newProcessor, newProcessor.Layers.findLayer(ID), 0, 10);
					break;
				case Qt::Key_A:
				case Qt::Key_Left:
					newProcessor.movePrimitive(newProcessor, newProcessor.Layers.findLayer(ID), -10, 0);
					break;
				case Qt::Key_D:
				case Qt::Key_Right:
					newProcessor.movePrimitive(newProcessor, newProcessor.Layers.findLayer(ID), 10, 0);
					break;
				case Qt::Key_Plus:
					newProcessor.ImgScale(newProcessor, newProcessor.Layers.findLayer(ID), 1.1);
					break;
				case Qt::Key_Minus:
					newProcessor.ImgScale(newProcessor, newProcessor.Layers.findLayer(ID), 0.9);
					break;
				}
			}
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "warning", "CHANNEL WRONG", QMessageBox::Yes, QMessageBox::Yes);
	}
}

void MainWindow::on_createpri_clicked() {
	mouseState = 2;
}

void MainWindow::on_addText_clicked() {
	QString temp = ui->textEdit->text();
	ui->textEdit->clear();
	FondFace fondF = CV_FONT_HERSHEY_SIMPLEX;
	if (ui->comboBox->currentIndex() == 1)  fondF = CV_FONT_HERSHEY_TRIPLEX;
	else if (ui->comboBox->currentIndex() == 2) fondF = CV_FONT_HERSHEY_SCRIPT_SIMPLEX;

	Layer* L = new Layer(temp.toStdString(), 200, 100);
	newProcessor.Layers.addLayerAsTop(*L);

	
	nowid++;
	QString title;
	title.setNum(nowid);//int to QString
	QByteArray ba = title.toLatin1();
	const char* ch = ba.data();//QString to char*
	ui->toolBox->insertItem(0 ,new QLabel(ch), tr(ch));

	
	newProcessor.changeTextColor(newProcessor, newProcessor.Layers.findLayer(nowid), ui->slider_b->value(), ui->slider_g->value(), ui->slider_r->value());
	newProcessor.changeTextFace(newProcessor, newProcessor.Layers.findLayer(nowid), fondF);
	newProcessor.changeTextScale(newProcessor, newProcessor.Layers.findLayer(nowid), ui->scaleBox->value());
	newProcessor.changeTextThickness(newProcessor, newProcessor.Layers.findLayer(nowid), 1);
	
}