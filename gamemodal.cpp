#include "gamemodal.h"
#include "ui_gamemodal.h"
#include "mainwindow.h"

// Emit new volume to the main window
void GameModal::musicChange() {emit newMusicVolume(ui->musicBar->value());}
void GameModal::soundChange() {emit newSoundVolume(ui->soundBar->value());}

// ESC should close the modal
void GameModal::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit gameModalClosed();
        this->close();
    }
}

// This modal opens when you press ESC in game
GameModal::GameModal(float music, float sound, QWidget *parent) : QDialog(parent) , ui(new Ui::GameModal) {
    ui->setupUi(this);

    // These will make the dialog look like a modal and make sure the modal will be deleted after it's closed
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setModal(true);

    ui->musicBar->setValue(music);
    ui->soundBar->setValue(sound);

    connect(ui->musicBar, &QSlider::valueChanged, this, &GameModal::musicChange);
    connect(ui->soundBar, &QSlider::valueChanged, this, &GameModal::soundChange);
    connect(this, &GameModal::newMusicVolume, qobject_cast<MainWindow*>(this->parent()), &MainWindow::changeMusicVolume);
    connect(this, &GameModal::newSoundVolume, qobject_cast<MainWindow*>(this->parent()), &MainWindow::changeSoundVolume);

    connect(ui->backButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::exitGame);
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(this, &GameModal::gameModalClosed, qobject_cast<MainWindow*>(this->parent()), &MainWindow::gameModalClosed);

    connect(ui->closeButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::gameModalClosed);
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

}

GameModal::~GameModal() {
    delete ui;
}
