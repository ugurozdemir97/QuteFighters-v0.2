#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonObject>
#include <QLineEdit>
#include <QJsonDocument>

// Send your Id when you connect to the server or disconnect from the server
void MainWindow::connection () {
    // This will be changed if you recieve a message about connection from the server
    ui->connectionInfoLabel->setText("Cannot connect to the server :(");

    QJsonObject obj;
    if (this->connectedServer == true) {
        obj["type"] = 8;  // If you are connected this will disconnect you
    }

    obj["username"] = this->username;
    obj["id"] = this->id;
    QByteArray data = QJsonDocument(obj).toJson();
    this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
}

// All the recieved messages are handled here
void MainWindow::getMessage() {
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;

    buffer.resize(socket->pendingDatagramSize());
    this->socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    // Check if the message is from this application
    if (sender == QHostAddress::LocalHost && senderPort == socket->localPort()) return;

    QJsonObject message = QJsonDocument::fromJson(buffer).object();

    switch (message["type"].toInt()) {
        case 0:  // These messages should be sent in game because these should be the fastest
            if (message["player"].toInt() == 0) {
                this->movePlayerOnline(*this->player1, message["press"].toBool(), message["key"].toInt(), this->opponentPressedKeys);
            } else {
                this->movePlayerOnline(*this->player2, message["press"].toBool(), message["key"].toInt(), this->opponentPressedKeys);
            }
            break;

        case 1:
            if (this->selection->currentPlayerNumber == 1) {
                this->movePlayerOnline(*this->player1, message["press"].toBool(), message["key"].toInt(), this->pressedKeys);
            } else {
                this->movePlayerOnline(*this->player2, message["press"].toBool(), message["key"].toInt(), this->pressedKeys);
            }
            break;
        case 2: { // Chatting
            this->chatLine->setText("");
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(message["message"].toString());
            this->chatArea->addItem(newItem);
            this->chatArea->scrollToBottom();
            break;
        }
        case 3:  // Create room
            this->currentRoom = message["room"].toInt();
            this->playerStatus = true;
            this->waitingRoom(true, false);  // Open waiting room and create one selector
            this->selection->currentPlayerNumber = 1;
            this->selection->placePlayer1(this->username, 0);
            this->startButton->show();
            break;
        case 4:  // Join room
            this->currentRoom = message["room"].toInt();
            this->waitingRoom(false, false);
            this->roleChange();  // Get information about the players and roles and place them
            break;
        case 5:  // Exit room
            this->exitWaitingRoom();
            break;
        case 6: { // Role change, general message that shows the new players in a room

            QJsonArray players = message["roleChange"].toArray();

            // If there are no players, Node can't send an empty array, so I send 3 item instead
            if (players.size() == 3) {

                // Delete everything
                this->selection->deleteSelector();
                this->selection->currentPlayerNumber = 0;
                this->playerStatus = false;
                this->activateStartButton(false);
                this->startButton->hide();

                this->selection->deleteDisplay(1);
                this->selection->readyPlayer1 = false;
                this->selection->player1Selects = -1;

                this->selection->deleteDisplay(2);
                this->selection->readyPlayer2 = false;
                this->selection->player2Selects = -1;

            // If there is one player
            } else if (players.size() == 1) {


                QJsonObject player1 = players[0].toObject();

                // If that player is us, then there are 5 options:
                // 1-) We were player 2 but the player 1 left
                // 2-) We joined the game
                // 3-) We select or deselect a character, we don't need to do something for this option
                // 4-) Player 2 left

                if (player1["username"].toString() == this->username && player1["port"].toString() == QString::number(this->socket->localPort())) {

                    // If we used to be the second player move us from second to the first player position
                    if (this->selection->currentPlayerNumber == 2) {

                        this->selection->deleteDisplay(1);

                        this->selection->readyPlayer1 = this->selection->readyPlayer2;
                        this->selection->player1Selects = this->selection->player2Selects;
                        this->selection->currentPlayerNumber = 1;

                        this->selection->placePlayer1(this->username, this->selection->player1Selects);
                        this->startButton->show();

                    // If we just joined the game
                    } else if (this->selection->currentPlayerNumber == 0) {
                        this->selection->currentPlayerNumber = 1;
                        this->selection->readyPlayer1 = false;
                        this->selection->player1Selects = 0;
                        this->playerStatus = true;
                        this->selection->createSelector(true, false);
                        this->selection->placePlayer1(this->username, 0);
                    }

                // If we are not that player, place that player's info to our client
                } else {
                    this->selection->deleteSelector();
                    this->selection->deleteDisplay(1);
                    this->selection->currentPlayerNumber = 0;
                    this->playerStatus = false;
                    this->startButton->hide();

                    if (player1["character"].toString() == "null") {
                        this->selection->readyPlayer1 = false;
                        this->selection->placePlayer1(player1["username"].toString(), -1);
                    } else {
                        this->selection->readyPlayer1 = true;
                        this->selection->player1Selects = player1["character"].toString().toInt();
                        this->selection->placePlayer1(player1["username"].toString(), player1["character"].toString().toInt());
                    }
                }

                // The start button should be deactive, everything about player 2 should be deleted
                this->activateStartButton(false);
                this->selection->deleteDisplay(2);
                this->selection->readyPlayer2 = false;
                this->selection->player2Selects = -1;

            // If there are 2 players
            } else {
                QJsonObject player1 = players[0].toObject();
                QJsonObject player2 = players[1].toObject();

                // If the change happend on player one, we don't need to do anything
                // If the change happened on player 2, all we need to do is deleting player 2 and recreating it
                if (player1["username"].toString() == this->username && player1["port"].toString() == QString::number(this->socket->localPort())) {

                    this->selection->deleteDisplay(2);

                    if (player2["character"].toString() == "null") {
                        this->selection->readyPlayer2 = false;
                        this->activateStartButton(false);
                        this->selection->placePlayer2(player2["username"].toString(), -1);
                    } else {
                        this->selection->readyPlayer2 = true;
                        this->selection->player2Selects = player2["character"].toString().toInt();
                        if (this->selection->readyPlayer1) this->activateStartButton(true);
                        this->selection->placePlayer2(player2["username"].toString(), player2["character"].toString().toInt());
                    }

                // If we are player 2, if the change happened on player 1, just delete it and recreate it.
                // If the change happened on player 2, which is us, then there are 2 options
                // 1-) We selected and deselected a character, in this situation we don't need to do anything
                // 2-) We have just joined the game, then we need to create a selector
                } else if (player2["username"].toString() == this->username && player2["port"].toString() == QString::number(this->socket->localPort())) {

                    this->activateStartButton(false);
                    this->startButton->hide();

                    // If we were already player 2, it means the change must be a character selection or deselection
                    // If it is from us, we don't need to do anything, in case it is from player 1, delete and recreate player 1 again

                    if (this->selection->currentPlayerNumber == 2) {
                        this->selection->deleteDisplay(1);

                        if (player1["character"].toString() == "null") {
                            this->selection->readyPlayer1 = false;
                            this->selection->placePlayer1(player1["username"].toString(), -1);
                        } else {
                            this->selection->readyPlayer1 = true;
                            this->selection->player1Selects = player1["character"].toString().toInt();
                            this->selection->placePlayer1(player1["username"].toString(), player1["character"].toString().toInt());
                        }

                    // If we were a spectator and just joined the game or just join a room
                    } else if (this->selection->currentPlayerNumber == 0) {

                        if (this->selection->player1Name == nullptr) {
                            if (player1["character"].toString() == "null") {
                                this->selection->readyPlayer1 = false;
                                this->selection->placePlayer1(player1["username"].toString(), -1);
                            } else {
                                this->selection->readyPlayer1 = true;
                                this->selection->player1Selects = player1["character"].toString().toInt();
                                this->selection->placePlayer1(player1["username"].toString(), player1["character"].toString().toInt());
                            }
                        }
                        
                        // If we just join the room, I should add player 1 here 
                        this->selection->currentPlayerNumber = 2;
                        this->selection->readyPlayer2 = false;
                        this->selection->player2Selects = 0;
                        this->playerStatus = true;
                        this->selection->createSelector(true, false);
                        this->selection->placePlayer2(this->username, 0);
                    }

                // If we are not a player, then delete all and recreate it
                } else {

                    this->selection->deleteSelector();
                    this->selection->currentPlayerNumber = 0;
                    this->playerStatus = false;
                    this->activateStartButton(false);
                    this->startButton->hide();

                    this->selection->deleteDisplay(1);
                    this->selection->readyPlayer1 = false;
                    this->selection->player1Selects = -1;

                    this->selection->deleteDisplay(2);
                    this->selection->readyPlayer2 = false;
                    this->selection->player2Selects = -1;

                    if (player1["character"].toString() == "null") {
                        this->selection->readyPlayer1 = false;
                        this->selection->placePlayer1(player1["username"].toString(), -1);
                    } else {
                        this->selection->readyPlayer1 = true;
                        this->selection->player1Selects = player1["character"].toString().toInt();
                        this->selection->placePlayer1(player1["username"].toString(), player1["character"].toString().toInt());
                    }

                    if (player2["character"].toString() == "null") {
                        this->selection->readyPlayer2 = false;
                        this->activateStartButton(false);
                        this->selection->placePlayer2(player2["username"].toString(), -1);
                    } else {
                        this->selection->readyPlayer2 = true;
                        this->selection->player2Selects = player2["character"].toString().toInt();
                        this->selection->placePlayer2(player2["username"].toString(), player2["character"].toString().toInt());
                    }
                }
            }

            break;
        }
        case 7:  // Username changed
            ui->usernameButton->setText(this->username);
            break;
        case 8:  // Disconnected
            this->connectedServer = false;
            this->connected();
            break;
        case 9:  // Connection succesful
            this->connectedServer = true;
            this->connected();
            break;
        case 10:
            this->gameStart();
            break;
        default:
            break;
    }
}

// Inform the user about connection status
void MainWindow::connected() {
    if (this->connectedServer == true) {
        QString style= "QPushButton#connectButton "
                        "{border: none;border-radius: 5px;"
                        "background: qradialgradient(cx: 0.5, cy: 0.5, radius: 1,fx: 0.5, fy: 0.5,stop: 0 #00ff00, stop: 1 #007800);}"
                        "QPushButton#connectButton:hover "
                        "{background: qradialgradient(cx: 0.5, cy: 0.5, radius: 1,fx: 0.5, fy: 0.5,stop: 0 #00ff00, stop: 1 #004200);}";
        ui->connectionInfoLabel->setText("Succesfully connected!");
        ui->connectButton->setStyleSheet(style);
    } else {
        QString style= "QPushButton#connectButton "
                        "{border: none;border-radius: 5px;"
                        "background: qradialgradient(cx: 0.5, cy: 0.5, radius: 1,fx: 0.5, fy: 0.5,stop: 0 #ff0000, stop: 1 #780000);}"
                        "QPushButton#connectButton:hover "
                        "{background: qradialgradient(cx: 0.5, cy: 0.5, radius: 1,fx: 0.5, fy: 0.5,stop: 0 #ff0000, stop: 1 #420000);}";
        ui->connectionInfoLabel->setText("Disconnected");
        ui->connectButton->setStyleSheet(style);
    }
}

// Create room
void MainWindow::createRoom () {
    if (this->currentRoom != 0) return;  // Return if you are already in a room and somehow send create room request
    if (this->connectedServer == true) { // If connected, send create room request
        QJsonObject obj;
        obj["type"] = 2;
        obj["id"] = this->id;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Select or deselect character
void MainWindow::characterChange() {
    if (this->currentRoom == 0 || this->selection->currentPlayerNumber == 0) return;  // Return if you are not in a room or not a player
    if (this->connectedServer == true) { // If connected, send character change request
        QJsonObject obj;
        obj["type"] = 9;
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;

        // If we didn't select a character send false, otherwise send which character we chose
        if (this->selection->currentPlayerNumber == 1) {
            if (this->selection->readyPlayer1) obj["character"] = this->selection->player1Selects;
            else obj["character"] = false;
        } else if (this->selection->currentPlayerNumber == 2) {
            if (this->selection->readyPlayer2) obj["character"] = this->selection->player2Selects;
            else obj["character"] = false;
        }
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Leave player status
void MainWindow::leaveGame () {
    if (this->currentRoom == 0) return;
    if (this->connectedServer == true) {
        QJsonObject obj;
        obj["type"] = 6;
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Get player status
void MainWindow::joinGame () {
    if (this->currentRoom == 0) return;
    if (this->connectedServer == true) { // If connected, send create room request
        QJsonObject obj;
        obj["type"] = 5;
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

void MainWindow::roleChange () {
    if (this->currentRoom == 0) return;
    if (this->connectedServer == true) { // If connected, send create room request
        QJsonObject obj;
        obj["type"] = 10;
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Exit room
void MainWindow::exitRoom () {
    if (this->currentRoom == 0) return;  // Return if you are not in a room
    if (this->connectedServer == true) { // If connected, send create room request
        QJsonObject obj;
        obj["type"] = 4;  // If you are connected this will disconnect you
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Join room
void MainWindow::joinRoom (int roomNumber) {
    if (this->currentRoom != 0) return;  // Return if you are already in a room and somehow send create room request
    if (this->connectedServer == true) { // If connected, send join room request
        QJsonObject obj;
        obj["type"] = 3;
        obj["id"] = this->id;
        obj["room"] = roomNumber;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Send message
void MainWindow::sendMessage () {
    if (this->currentRoom == 0) return;
    if (this->connectedServer == true && this->chatLine->text().length() != 0) { // If connected and the message is not empty send message
        QJsonObject obj;
        obj["type"] = 1;
        obj["message"] = this->chatLine->text();
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Send key press events
void MainWindow::sendKeyAction(KeyActions action, bool press) {
    if (this->currentRoom == 0) return;  // Return if you are not in a room
    if (this->connectedServer == true) { // If connected, send inputs to the server
        QJsonObject obj;
        obj["type"] = 0;
        obj["press"] = press;   // If false, it is key release. Otherwise, key press.
        obj["key"] = action;    // Which key we pressed for the opponent
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }
}

// Send key press events
void MainWindow::sendGameStart() {
    if (this->currentRoom == 0) {
        this->gameStart();
    } else {
        if (this->connectedServer == true) { // If connected, send join room request
            QJsonObject obj;
            obj["type"] = 11;
            obj["room"] = this->currentRoom;
            QByteArray data = QJsonDocument(obj).toJson();
            this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
        }
    }
}
