#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    server = new Server(this);
    ui->statusBar->addPermanentWidget(ui->statusLabel);
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

void MainWindow::updateUI(QList<User*> users) {
    ui->plainTextEdit->clear();
    foreach (User *u, users) {
        ui->plainTextEdit->appendPlainText(u->getUsername());
    }
    ui->statusLabel->setText(QString("Online users: " + users.length()));
}
