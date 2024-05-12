#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRandomGenerator>
#include <QAudioOutput>
#include <QDir>

// Everything related to music and sound

void MainWindow::changeVolume() {
    if (ui->musicButton->isChecked()) {
        this->musicVolume = ui->soundBar->value();
        this->musicOutput->setVolume(this->musicVolume / 100);
    } else this->soundVolume = ui->soundBar->value();
}

void MainWindow::changeMusicVolume(float volume) {
    this->musicVolume = volume;
    this->musicOutput->setVolume(volume / 100);
}
void MainWindow::changeSoundVolume(float volume) {this->soundVolume = volume;}

// Play sound effects
void MainWindow::playSound1(QString path) {
    if (this->soundVolume != 0) {
        this->soundPlayer1 = new QSoundEffect(this);
        this->soundPlayer1->setVolume(this->soundVolume / 100);
        this->soundPlayer1->setSource(QUrl(path));
        this->soundPlayer1->play();
    }
}

void MainWindow::playSound2(QString path) {
    if (this->soundVolume != 0) {
        this->soundPlayer2 = new QSoundEffect(this);
        this->soundPlayer2->setVolume(this->soundVolume / 100);
        this->soundPlayer2->setSource(QUrl(path));
        this->soundPlayer2->play();
    }
}

// Play the music again if the music finishes
void MainWindow::stateOfMusic() {
    if (this->musicPlayer->mediaStatus() == QMediaPlayer::EndOfMedia) {
        this->playMusic();
    }
}

// Play a random music from the music folder. In game music and UI music are different.
void MainWindow::playMusic() {

    this->musicPlayer->stop();

    int random;
    QString musicPath;

    if (ui->pages->currentIndex() == 3) {
        if (this->gameMusicList.size() == 0) return;
        random = QRandomGenerator::global()->bounded(this->gameMusicList.size());
        musicPath = "qrc" +  this->gameMusicList[random];
    } else {
        if (this->menuMusicList.size() == 0) return;
        random = QRandomGenerator::global()->bounded(this->menuMusicList.size());
        musicPath = "qrc" + this->menuMusicList[random];
    }

    this->musicPlayer->setSource(QUrl(musicPath));
    this->musicPlayer->play();

}

// Set up music player
void MainWindow::setUpMusic() {

    // Our music player
    this->musicPlayer = new QMediaPlayer;
    this->musicOutput = new QAudioOutput;
    this->musicPlayer->setAudioOutput(this->musicOutput);
    this->musicOutput->setVolume(this->musicVolume / 100);

    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::stateOfMusic);

    // Get all musics
    QDir musicFolder(":/Music/Menu/");
    QStringList musicNames = musicFolder.entryList(QStringList() << "*.mp3", QDir::Files);
    for (const QString &fileName : musicNames) {this->menuMusicList.append(musicFolder.absoluteFilePath(fileName));}

    QDir musicFolder2(":/Music/Fight/");
    QStringList musicNames2 = musicFolder2.entryList(QStringList() << "*.mp3", QDir::Files);
    for (const QString &fileName : musicNames2) {this->gameMusicList.append(musicFolder2.absoluteFilePath(fileName));}

    this->playMusic();
}
