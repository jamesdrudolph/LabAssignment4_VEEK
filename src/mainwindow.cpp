#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnOverlayForm_clicked()
{
    OverlayForm *ow = new OverlayForm;
    ow->setAttribute(Qt::WA_DeleteOnClose);
    ow->show();
}

void MainWindow::on_btnBrightnessForm_clicked()
{
    AdjustmentsForm *af = new AdjustmentsForm;
    af->setAttribute(Qt::WA_DeleteOnClose);
    af->show();
}
