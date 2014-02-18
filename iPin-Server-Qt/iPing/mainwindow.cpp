#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    line=ui->lineEdit;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetLineText(QString str)
{
    line->setText(str);
}
