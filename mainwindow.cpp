#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(new Server(this)),
    model(new UserTableModel(this))
{
    ui->setupUi(this);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setModel(model);
    model->setUsers(server->getUsers());
    connect(server, SIGNAL(usersChanged(QList<User*>*)), model, SLOT(setUsers(QList<User*>*)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    server->start();
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked() {
    server->shutdown();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}
