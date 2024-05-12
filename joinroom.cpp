#include "joinroom.h"
#include "ui_joinroom.h"
#include "mainwindow.h"

// Check if the string is only digits
bool isOnlyDigits(const QString& string) {
    for (const QChar& i : string) if (!i.isDigit()) return false;
    return true;
}

// ESC should close the modal
void JoinRoom::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit hideOverlay();
        this->close();
    }
}

// Check if the text is valid
void JoinRoom::isValid() {
    QString roomNumber = ui->lineEdit->text();
    if (roomNumber.length() != 6 || isOnlyDigits(roomNumber) == false) return;
    qobject_cast<MainWindow*>(parent())->joinRoom(ui->lineEdit->text().toInt());
    qobject_cast<MainWindow*>(parent())->hideOverlay();
    this->close();
}

// Handles JoinRoom modal
JoinRoom::JoinRoom(QWidget *parent) : QDialog(parent), ui(new Ui::JoinRoom) {
    ui->setupUi(this);

    // These will make the dialog look like a modal and make sure the modal will be deleted after it's closed
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QIntValidator *validator = new QIntValidator(000000, 999999, this);
    ui->lineEdit->setValidator(validator);
    ui->lineEdit->setMaxLength(6);

    connect(ui->enterButton, &QPushButton::clicked, this, &JoinRoom::isValid);
    connect(this, &JoinRoom::hideOverlay, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->cancelButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

}

JoinRoom::~JoinRoom() {
    delete ui;
}
