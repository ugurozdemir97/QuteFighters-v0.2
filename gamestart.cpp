#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "background.h"
#include "gamescene.h"
#include "gametime.h"
#include <QJsonObject>

void MainWindow::gameStart() {

    // QGraphicsView of the game
    this->gameView = new GameView(ui->pages->widget(3));
    this->gameView->setGeometry(0, 0, this->width(), this->height());

    // Create game scene class
    this->gameScene = new GameScene(this);
    this->gameView->setScene(this->gameScene);  // Set view

    // Background images
    this->scene1 = new Background(QPixmap(":/Images/Game/1.png"), this->scale);
    this->scene2 = new Background(QPixmap(":/Images/Game/2.png"), this->scale);
    this->scene3 = new Background(QPixmap(":/Images/Game/3.png"), this->scale);

    this->gameScene->addItem(this->scene1);
    this->gameScene->addItem(this->scene2);
    this->gameScene->addItem(this->scene3);

    // Scroll to the center of the scenes
    this->gameView->centerOn(this->gameScene->width() / 2.0, this->height() / 2.0);

    // Navigate
    ui->pages->setCurrentIndex(3);

    // Coordinates of the visible portion of the scene
    this->left = this->gameView->visiblePortion.left();
    this->right = this->gameView->visiblePortion.right();

    // How much the background can move
    this->sceneLimitLeft = -1950 * this->scale;  // This is the maximum left and right coordinates of a character can move
    this->sceneLimitRight = 1950 * this->scale;

    // Add the timers
    this->gameTime = new GameTime((left + right) / 2.0, right, this->height() / 2.0, this->scale, this);
    this->gameScene->addItem(this->gameTime->countDownText);
    this->gameScene->addItem(this->gameTime->gameTimeText);

    // Selected characters
    QJsonObject character1 = this->selection->characters[this->selection->player1Selects].toObject();
    QJsonObject character2 = this->selection->characters[this->selection->player2Selects].toObject();

    // Add the health bars
    this->healthBar1 = new HealthBar(QPixmap(":/Images/Game/HealthBar.png").transformed(QTransform().scale(-1, 1)),
                                     QPixmap(character1["avatar"].toString()).transformed(QTransform().scale(-1, 1)),
                                     character1["health"].toInt(),
                                     true, this->selection->player1Name->toPlainText(),
                                     this->scale, left, right);
    this->healthBar2 = new HealthBar(QPixmap(":/Images/Game/HealthBar.png"),
                                     QPixmap(character2["avatar"].toString()),
                                     character2["health"].toInt(),
                                     false, this->selection->player2Name->toPlainText(),
                                     this->scale, left, right);

    this->gameScene->addItem(this->healthBar1);
    this->gameScene->addItem(this->healthBar2);

    // Add the players
    this->player1 = new Player(QPixmap(character1["sprites"].toString()), character1, character2["speed"].toInt(), true,
                               this->scale, left, right, this->height());
    this->player2 = new Player(QPixmap(character2["sprites"].toString()), character2, character1["speed"].toInt(), false,
                               this->scale, left, right, this->height());

    this->gameScene->addItem(this->player1);
    this->gameScene->addItem(this->player2);

    // Sound playing
    connect(this->player1, &Player::playSound, this, &MainWindow::playSound1);
    connect(this->player2, &Player::playSound, this, &MainWindow::playSound2);
    this->musicPlayer->stop();

    // Players can't move outside of the visible portion of the QGraphicsScene
    this->player1->setLimits(this->player2->frameWidth, left, right, this->scale);
    this->player2->setLimits(this->player2->frameWidth, left, right, this->scale);

    // Players can't be more far away than this. We need this for moving the background if players are closer.
    this->maxDistance = (right - (this->player1->frameWidth * this->scale)) - left;

    this->gameTime->startTimers();

}

// Delete everything related to the game and turn back to the waiting room
void MainWindow::exitGame() {

    delete gameTime;
    delete scene1;
    delete scene2;
    delete scene3;
    delete healthBar1;
    delete healthBar2;
    delete player1;
    delete player2;
    delete gameScene;
    delete gameView;
    delete gameModal;
    delete soundPlayer1;
    delete soundPlayer2;

    gameTime = nullptr;
    scene1 = nullptr;
    scene2 = nullptr;
    scene3 = nullptr;
    healthBar1 = nullptr;
    healthBar2 = nullptr;
    player1 = nullptr;
    player2 = nullptr;
    gameScene = nullptr;
    gameView = nullptr;
    gameModal = nullptr;
    soundPlayer1 = nullptr;
    soundPlayer2 = nullptr;

    ui->pages->setCurrentIndex(2);
    this->FPStest = 1;
    this->playMusic();

}
