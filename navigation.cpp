#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "keys.h"
#include "username.h"
#include "joinroom.h"
#include "gamemodal.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QLineEdit>

// Navigate between pages and hiding / showing widgets

void MainWindow::backToMain() {
    // The border-images are always rendered because I noticed that if I hide one of them less memory is used
    // So I will only show one of them at a time.
    ui->pages->widget(1)->setStyleSheet(QString(""));
    ui->pages->widget(0)->setStyleSheet(QString("QWidget#mainPage {border-image: url(:/Images/UI/Main.png);}"));
    ui->pages->setCurrentIndex(0);
}

void MainWindow::aboutPage() {
    ui->pages->widget(0)->setStyleSheet(QString(""));
    ui->pages->widget(1)->setStyleSheet(QString("QWidget#aboutPage {border-image: url(:/Images/UI/Parchment.png);}"));
    ui->aboutLabel3->hide();
    ui->changeKeysButton->hide();
    ui->aboutLabel1->show();
    ui->aboutLabel2->show();
    ui->pages->setCurrentIndex(1);
}

void MainWindow::howToPage() {
    ui->pages->widget(0)->setStyleSheet(QString(""));
    ui->pages->widget(1)->setStyleSheet(QString("QWidget#aboutPage {border-image: url(:/Images/UI/Parchment.png);}"));
    ui->aboutLabel1->hide();
    ui->aboutLabel2->hide();
    ui->aboutLabel3->show();
    ui->changeKeysButton->show();
    ui->pages->setCurrentIndex(1);
}

void MainWindow::playPage() {
    ui->localButton->show();
    ui->joinButton->show();
    ui->createButton->show();
    ui->backButton2->show();
    ui->playButton->hide();
    ui->aboutButton->hide();
    ui->howToButton->hide();
}

void MainWindow::hidePlayPage() {
    ui->playButton->show();
    ui->aboutButton->show();
    ui->howToButton->show();
    ui->localButton->hide();
    ui->joinButton->hide();
    ui->createButton->hide();
    ui->backButton2->hide();
}

// Show music bar
void MainWindow::showMusicBar() {
    QObject* obj = sender();  // Which button did I click

    if(obj == ui->musicButton) {
        if (ui->musicButton->isChecked()) {
            ui->soundButton->setChecked(false);
            ui->soundBar->setValue(this->musicVolume);
            ui->soundBar->show();
            ui->soundBarLabel->show();
        }
    } else if(obj == ui->soundButton) {
        if (ui->soundButton->isChecked()) {
            ui->musicButton->setChecked(false);
            ui->soundBar->setValue(this->soundVolume);
            ui->soundBar->show();
            ui->soundBarLabel->show();
        }
    }

    if (!ui->soundButton->isChecked() && !ui->musicButton->isChecked()) {
        ui->soundButton->setChecked(false);
        ui->musicButton->setChecked(false);
        ui->soundBar->hide();
        ui->soundBarLabel->hide();
    }
}

// Show hide chat area
void MainWindow::showChatArea() {
    if (this->openChatButton->isChecked()) {
        this->chatArea->show();
        this->chatLine->show();
    } else {
        this->chatLine->clearFocus();
        this->chatArea->hide();
        this->chatLine->hide();
        this->chatLine->setText("");
    }
}

// Hide and show overlay
void MainWindow::showOverlay() {
    this->overlay = new QWidget(ui->centralwidget);
    this->overlay->setGeometry(0, 0, this->width(), this->height());
    this->overlay->setStyleSheet("background-color: rgba(0, 0, 0, 130);");
    this->overlay->show();
}

void MainWindow::hideOverlay() {
    this->overlay->hide();
    delete this->overlay;
    this->overlay = nullptr;
}

// Update username
void MainWindow::updateUsername() {
    // If you are connected to a server, inform the server about the username change
    if (this->connectedServer == true) {
        QJsonObject obj;
        obj["type"] = 7;
        obj["username"] = this->username;
        obj["id"] = this->id;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    } else {
        ui->usernameButton->setText(this->username);
    }

}

// Show join room modal
void MainWindow::showJoinModal() {
    if (this->connectedServer == true) {
        this->showOverlay();
        JoinRoom *dialog = new JoinRoom(this);
        dialog->show();
        dialog->move(dialog->x(), dialog->y() + 40);
    }
}

// Show username modal
void MainWindow::showUsernameModal() {
    this->showOverlay();
    Username *dialog = new Username(this->username, this);
    dialog->show();
    dialog->move(dialog->x(), dialog->y() + 40);
}

// Show Key bindings dialog
void MainWindow::showKeyModal() {
    this->showOverlay();
    Keys *dialog = new Keys(this->player1Keys, this->player2Keys, this);
    dialog->show();
    dialog->move(dialog->x(), dialog->y() + 40);
}

// Show Game Modal when you press ESC in game
void MainWindow::showGameModal() {
    this->gameModal = new GameModal(this->musicVolume, this->soundVolume, this);
    this->gameModal->show();
    this->gameModal->move(this->gameModal->x(), this->gameModal->y() + 40);
}
