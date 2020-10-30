#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mytcp.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SimpleTcpSocketServerDemo server;
    SimpleTcpSocketClientDemo client;


}

MainWindow::~MainWindow()
{
    delete ui;
}

