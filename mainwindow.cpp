#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    server = new MyServer(this);
    connect(server, SIGNAL(dataReady(QByteArray)), this, SLOT(writeData(QByteArray)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    server->start();
}

void MainWindow::on_pushButton_2_clicked() {
    server->shutdown();
}

void MainWindow::writeData(QByteArray Data) {
    ui->plainTextEdit->appendPlainText(Data);
}
