#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(new Server(this)),
    model(new UserTableModel(this))
{
    ui->setupUi(this);
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
    /*QSqlQuery q;
    q.prepare("SELECT * FROM users");
    q.exec();
    q.next();
    qDebug() << q.value(0) << q.value(1);*/
}

void MainWindow::on_pushButton_2_clicked() {
    server->shutdown();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}
