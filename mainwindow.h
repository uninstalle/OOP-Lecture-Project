#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void keyPressEvent(QKeyEvent* e);
private slots:
	void on_save_clicked();
	void on_actionBack_triggered();
	void action_open();
	void on_flipHor_clicked();
	void on_flipVer_clicked();
    void on_addLayer_clicked();
    void on_deleteLayer_clicked();
    void on_layerUp_clicked();
    void on_layerDown_clicked();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
    void on_NostalgicHue_triggered();
    void on_StrongLight_triggered();
    void on_DarkTown_triggered();
    void on_Feather_triggered();
    void on_Mosaic_triggered();
    void on_Sculpture_triggered();
	void on_merge_clicked();
    void on_Diffusion_triggered();
    void on_GaussianBlur_triggered();
    void on_Wind_triggered();
    void on_horizontalSlider_2_valueChanged(int value);
    void on_spinBox_2_valueChanged(int arg1);
	void on_spinBox_angle_valueChanged(int arg1);
	void on_slider_angle_valueChanged(int value);
	void timerUpDate();
	void on_magicWand_btn_clicked();
	void on_createpri_clicked();
	void on_addText_clicked();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;
    QPixmap pix;
};

#endif // MAINWINDOW_H
