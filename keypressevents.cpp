#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>

void MainWindow::changeKeyBindings() {

    this->player1Methods = {};
    this->player2Methods = {};

    for (auto i = this->player1Keys.begin(); i != this->player1Keys.end(); ++i) {
        this->player1Methods[i.value()] = i.key();
    }

    for (auto i = this->player2Keys.begin(); i != this->player2Keys.end(); ++i) {
        this->player2Methods[i.value()] = i.key();
    }

}

// Key release on MainWindow
void MainWindow::keyReleaseEvent(QKeyEvent *event) {

    if (ui->pages->currentIndex() == 3 && this->playerStatus) {

        if (event->isAutoRepeat()) return;  // This is important! This prevents getting signals from keys multiple times

        if (this->currentRoom == 0) {
            this->movePlayer(*this->player1, false, event->key(), player1Keys, player1Methods);
            this->movePlayer(*this->player2, false, event->key(), player2Keys, player2Methods);
        } else {
            if (player1Methods.contains(event->key())) {
                this->sendKeyAction(player1Methods[event->key()], false);
            }
        }
    }
};

void MainWindow::keyPressEvent(QKeyEvent *event) {

    switch (ui->pages->currentIndex()) {
        case 3:

            if (this->playerStatus == false) return;
            if (event->isAutoRepeat()) return;  // Important!

            if (event->key() == Qt::Key_Escape && this->gameModal == nullptr) {
                if (this->currentRoom == 0) this->gameTime->stopTimers();
                this->showGameModal();
            }

            if (this->currentRoom == 0) {
                this->movePlayer(*this->player1, true, event->key(), player1Keys, player1Methods);
                this->movePlayer(*this->player2, true, event->key(), player2Keys, player2Methods);
            } else {
                if (player1Methods.contains(event->key())) {
                    this->sendKeyAction(player1Methods[event->key()], true);
                }
            }

            break;
        case 2:

            if (this->chatArea->isVisible()) {
                if (event->key() == Qt::Key_Return) this->sendMessage();
                if (event->key() == Qt::Key_Escape) {
                    this->openChatButton->setChecked(false);
                    this->showChatArea();
                }
                return;
            }

            if (this->selection->selector1 != nullptr && this->playerStatus) {
                switch (this->player1Methods[event->key()]) {
                    case Left:
                        if (this->currentRoom == 0) {
                            this->selection->moveSelector(3, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                            this->selection->displayCharacter1();
                        } else {
                            if (this->selection->currentPlayerNumber == 1) {
                                this->selection->moveSelector(3, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                                this->selection->displayCharacter1();
                            } else {
                                this->selection->moveSelector(3, *this->selection->selector1, this->selection->player2Selects, this->selection->readyPlayer2);
                                this->selection->displayCharacter2();
                            }
                        }
                        break;
                    case Right:
                        if (this->currentRoom == 0) {
                            this->selection->moveSelector(1, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                            this->selection->displayCharacter1();
                        } else {
                            if (this->selection->currentPlayerNumber == 1) {
                                this->selection->moveSelector(1, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                                this->selection->displayCharacter1();
                            } else {
                                this->selection->moveSelector(1, *this->selection->selector1, this->selection->player2Selects, this->selection->readyPlayer2);
                                this->selection->displayCharacter2();
                            }
                        }
                        break;
                    case Crouch:
                        if (this->currentRoom == 0) {
                            this->selection->moveSelector(2, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                            this->selection->displayCharacter1();
                        } else {
                            if (this->selection->currentPlayerNumber == 1) {
                                this->selection->moveSelector(2, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                                this->selection->displayCharacter1();
                            } else {
                                this->selection->moveSelector(2, *this->selection->selector1, this->selection->player2Selects, this->selection->readyPlayer2);
                                this->selection->displayCharacter2();
                            }
                        }
                        break;
                    case Jump:
                        if (this->currentRoom == 0) {
                            this->selection->moveSelector(0, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                            this->selection->displayCharacter1();
                        } else {
                            if (this->selection->currentPlayerNumber == 1) {
                                this->selection->moveSelector(0, *this->selection->selector1, this->selection->player1Selects, this->selection->readyPlayer1);
                                this->selection->displayCharacter1();
                            } else {
                                this->selection->moveSelector(0, *this->selection->selector1, this->selection->player2Selects, this->selection->readyPlayer2);
                                this->selection->displayCharacter2();
                            }
                        }
                        break;
                    case LA:
                        if (this->selection->readyPlayer1 && this->selection->readyPlayer2) {
                            this->sendGameStart();
                        } else {
                            if (this->currentRoom == 0) this->selection->selectCharacter1();
                            else {
                                if (this->selection->currentPlayerNumber == 1) {
                                    this->selection->selectCharacter1();
                                    this->characterChange();
                                }
                                else {
                                    this->selection->selectCharacter2();
                                    this->characterChange();
                                }
                            }
                        }
                        break;
                    case HA:
                        if (this->currentRoom == 0) {
                            if (this->selection->readyPlayer1) this->selection->deselectCharacter1();
                            else {
                                this->exitWaitingRoom();
                                return;
                            }
                        } else {
                            if (this->selection->currentPlayerNumber == 1) {
                                if (this->selection->readyPlayer1) {
                                    this->selection->deselectCharacter1();
                                    this->characterChange();
                                } else {
                                    this->selection->deleteSelector();  // We became a sectator
                                    this->leaveGame();
                                }
                            } else {
                                if (this->selection->readyPlayer2) {
                                    this->selection->deselectCharacter2();
                                    this->characterChange();
                                } else {
                                    this->selection->deleteSelector();  // We became a sectator
                                    this->leaveGame();
                                }
                            }
                        }
                    default:
                        break;
                }

                if (event->key() == Qt::Key_Escape) {
                    if (this->currentRoom == 0) {
                        if (this->selection->readyPlayer1) this->selection->deselectCharacter1();
                        else {
                            this->exitWaitingRoom();
                            return;
                        }
                    } else {
                        if (this->selection->currentPlayerNumber == 1) {
                            if (this->selection->readyPlayer1) {
                                this->selection->deselectCharacter1();
                                this->characterChange();
                            } else {
                                this->selection->deleteSelector();  // We became a sectator
                                this->leaveGame();
                            }
                        } else {
                            if (this->selection->readyPlayer2) {
                                this->selection->deselectCharacter2();
                                this->characterChange();
                            } else {
                                this->selection->deleteSelector();  // We became a sectator
                                this->leaveGame();
                            }
                        }
                    }
                } else if (event->key() == Qt::Key_Return) {
                    if (this->selection->readyPlayer1 && this->selection->readyPlayer2) this->sendGameStart();
                }

            } else {
                // If we are a spectator, exit room and notify the server
                if (this->player1Methods[event->key()] == HA || event->key() == Qt::Key_Escape) {
                    this->exitWaitingRoom();
                    return;
                } else if (this->player1Methods[event->key()] == LA) {
                    this->joinGame();
                    return;
                }
            }

            if (this->selection->selector2 != nullptr && this->playerStatus) {
                switch (this->player2Methods[event->key()]) {
                    case Left:
                        this->selection->moveSelector(3, *this->selection->selector2, this->selection->player2Selects, this->selection->readyPlayer2);
                        this->selection->displayCharacter2();
                        break;
                    case Right:
                        this->selection->moveSelector(1, *this->selection->selector2, this->selection->player2Selects, this->selection->readyPlayer2);
                        this->selection->displayCharacter2();
                        break;
                    case Crouch:
                        this->selection->moveSelector(2, *this->selection->selector2, this->selection->player2Selects, this->selection->readyPlayer2);
                        this->selection->displayCharacter2();
                        break;
                    case Jump:
                        this->selection->moveSelector(0, *this->selection->selector2, this->selection->player2Selects, this->selection->readyPlayer2);
                        this->selection->displayCharacter2();
                        break;
                    case LA:
                        this->selection->selectCharacter2();
                        break;
                    case HA:
                        this->selection->deselectCharacter2();
                        break;
                    default:
                        break;
                }
            }

            break;
        case 1:
            if (event->key() == Qt::Key_Escape) this->backToMain();
            break;
        case 0:
            if (event->key() == Qt::Key_Escape) {
                if (ui->playButton->isVisible()) this->close();
                else this->hidePlayPage();
            }
            break;
        default:
            break;
    }
}

void MainWindow::movePlayer(Player &player, bool press, int key, QMap<KeyActions, int> keys, QMap<int, KeyActions> methods) {

    if (press) {
        this->pressedKeys.insert(key);  // Insert pressed keys to the QSet

        switch (methods[key]) {
            case Left:
                player.moveLeft();
                player.moveKeyLeft = true;
                break;
            case Right:
                player.moveRight();
                player.moveKeyRight = true;
                break;
            case Crouch:
                player.moveKeyDown = true;
                player.crouch();
                break;
            case Jump:
                player.jump();
                break;
            case LA:
                if (pressedKeys.contains(keys[Left]) || pressedKeys.contains(keys[Right])) player.sL();
                else if (pressedKeys.contains(keys[Crouch])) player.dL();
                else player.nL();
                break;
            case HA:
                if (pressedKeys.contains(keys[Left]) || pressedKeys.contains(keys[Right])) player.sH();
                else if (pressedKeys.contains(keys[Crouch])) player.dH();
                else player.nH();
                break;
            case Block:
                if (pressedKeys.contains(keys[Crouch])) player.crouchBlock();
                else player.block();
                break;
            default:
                break;
        }

    } else {
        this->pressedKeys.remove(key);  // Remove the released key from pressed keys

        switch (methods[key]) {
            case Left:
                player.moveKeyLeft = false;
                if (player.moveKeyRight) player.moveRight();
                else player.idle();
                break;
            case Right:
                player.moveKeyRight = false;
                if (player.moveKeyLeft) player.moveLeft();
                else player.idle();
                break;
            case Crouch:
                player.moveKeyDown = false;
                player.stand();
                break;
            case Block:
                if (pressedKeys.contains(keys[Crouch])) {
                    player.doing = Player::Crouch;
                    player.currentAnimation = player.animations["crouch"].size() - 1;
                } else player.idle();
                break;
            default:
                break;
        }
    }

}

void MainWindow::movePlayerOnline(Player &player, bool press, int key, QSet<int> &keys) {

    if (press) {
        keys.insert(key);  // Insert pressed keys to the QSet

        switch (key) {
            case 1:
                player.moveLeft();
                player.moveKeyLeft = true;
                break;
            case 2:
                player.moveRight();
                player.moveKeyRight = true;
                break;
            case 3:
                player.moveKeyDown = true;
                player.crouch();
                break;
            case 4:
                player.jump();
                break;
            case 5:
                if (keys.contains(1) || keys.contains(2)) player.sL();
                else if (keys.contains(3)) player.dL();
                else player.nL();
                break;
            case 6:
                if (keys.contains(1) || keys.contains(2)) player.sH();
                else if (keys.contains(3)) player.dH();
                else player.nH();
                break;
            case 7:
                if (keys.contains(3)) player.crouchBlock();
                else player.block();
                break;
            default:
                break;
        }

    } else {

        keys.remove(key);

        switch (key) {
            case 1:
                player.moveKeyLeft = false;
                if (player.moveKeyRight) player.moveRight();
                else player.idle();
                break;
            case 2:
                player.moveKeyRight = false;
                if (player.moveKeyLeft) player.moveLeft();
                else player.idle();
                break;
            case 3:
                player.moveKeyDown = false;
                player.stand();
                break;
            case 7:
                if (keys.contains(3)) {
                    player.doing = Player::Crouch;
                    player.currentAnimation = player.animations["crouch"].size() - 1;
                } else player.idle();
                break;
            default:
                break;
        }
    }
}
