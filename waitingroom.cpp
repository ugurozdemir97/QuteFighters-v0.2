#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "selection.h"
#include "selectionview.h"
#include <QLineEdit>

void MainWindow::waitingRoom(bool player1, bool player2) {

    this->selectionView->show();

    QObject* obj = sender();  // Which button did I click

    // If the game is online, show chat
    if(obj != ui->localButton) {
        this->openChatButton->show();
        this->selectionView->setEnabled(true);
    } else {
        this->selectionView->setEnabled(false);  // Prevent selecting characters with mouse
        this->playerStatus = true;               // This is important in keypress events
        this->startButton->show();               // This button will only be visible in local and if you are the room leader in online
    }

    this->exitRoomButton->show();

    // Create selection class
    this->selection = new Selection(this->scale, this->currentRoom, this);
    this->selection->createSelector(player1, player2);  // Create 1, 2 or 0 selectors 
    this->selectionView->setScene(this->selection);  // Set view

    // Connections
    connect(this->selection, &Selection::charactersReady, this, &MainWindow::activateStartButton);

    ui->pages->setCurrentIndex(2);

}

// Reset everything
void MainWindow::exitWaitingRoom() {

    this->exitRoom();

    // There will be problems in resizing the scene and its items if: you are in a room, resize the window, leave and enter back
    // This line prevents it
    this->selectionView->setTransform(QTransform::fromScale(1, 1));

    this->selectionView->hide();
    this->openChatButton->hide();
    this->openChatButton->setChecked(false);
    this->exitRoomButton->hide();
    this->chatArea->clear();
    this->chatArea->hide();
    this->startButton->hide();
    this->chatLine->hide();
    this->chatLine->clearFocus();
    this->chatLine->setText("");
    this->activateStartButton(false);
    this->playerStatus = false;
    this->currentRoom = 0;

    ui->pages->setCurrentIndex(0);

    delete this->selection;
    this->selection = nullptr;
}

void MainWindow::activateStartButton(bool ready) {
    if (ready) this->startButton->setEnabled(true);
    else this->startButton->setEnabled(false);
}
