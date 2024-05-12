#include "username.h"
#include "ui_username.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "mainwindow.h"
#include <QRandomGenerator>

// Save the username if it is longer than 2 characters and less than 16 characters
void Username::saveUsername() {
    if (ui->lineEdit->text().length() >= 2 && ui->lineEdit->text().length() <= 16) {
        this->username = ui->lineEdit->text();
    }
}

// ESC should close the modal
void Username::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit hideOverlay();
        this->close();
    } else if (event->key() == Qt::Key_Return) {
        ui->enterButton->click();
    }
}

// Initialize Username modal
Username::Username(QString &name, QWidget *parent) : QDialog(parent), ui(new Ui::Username), username(name) {
    ui->setupUi(this);

    // These will make the dialog look like a modal and make sure the modal will be deleted after it's closed
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setModal(true);

    // Only letters and digits, maximum 10 characters
    QRegularExpression regExp("[A-Za-z0-9]{2,16}");
    ui->lineEdit->setValidator(new QRegularExpressionValidator(regExp, this));
    ui->lineEdit->setText(this->username);

    // Close the dialog. If "Enter" change the username, otherwise use the random username
    connect(ui->enterButton, &QPushButton::clicked, this, &Username::saveUsername);
    connect(ui->enterButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::updateUsername);
    connect(ui->enterButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->enterButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(this, &Username::hideOverlay, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->skipButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->skipButton, SIGNAL(clicked()), this, SLOT(close()));
}

Username::~Username() {delete ui;}
