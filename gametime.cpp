#include "gametime.h"
#include "mainwindow.h"
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QProcess>

void GameTime::quitThread() {
    this->stopTimers();

    // Without waiting a little bit before closing the thread I get a strange message:
    // QObject::killTimer: Timers cannot be stopped from another thread
    // QObject::~QObject: Timers cannot be stopped from another thread
    // I've seen people have this problem but as far as I see nobody found a solution
    // I realized that if I had a qDebug() exactly here, the error message dissapears.
    // Instead of a qDebug(), I used a sleep method and wait just a little bit. Now the problem is solved

    QThread::msleep(40);

    if (this->gameThread->isRunning()) {
        this->gameThread->requestInterruption();
        this->gameThread->quit();
        this->gameThread->wait();
    }
}

// Start timers
void GameTime::startTimers() {
    if (this->countDownFrom > -1) {
        this->countDown->start(1000);
    } else if (this->gameTimeFrom > -1) {
        this->gameTime->start(1000);
        QMetaObject::invokeMethod(this->gameLoop, "start", Qt::QueuedConnection, Q_ARG(int, this->FPS));
    }
}

// Stop timers
void GameTime::stopTimers() {
    if (this->countDown->isActive()) {
        QMetaObject::invokeMethod(this->countDown, "stop", Qt::QueuedConnection);
    } else if (this->gameTime->isActive()) {
        QMetaObject::invokeMethod(this->gameTime, "stop", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this->gameLoop, "stop", Qt::QueuedConnection);
    }
}

// Change the text of the count down
void GameTime::displayCountDown() {

    QString text;
    this->countDownFrom --;

    switch(this->countDownFrom) {
        case 2:
        case 1:
            text = QString::number(this->countDownFrom);
            break;
        case 0:
            text = "Fight!";
            break;
        default:
            text = "";
    }

    this->countDownText->setPlainText(text);
    this->countDownText->setX(centerX - this->countDownText->boundingRect().width() * (15 * this->scale) / 2.0);

    // When the countdown ends start the game loop and game time
    if (this->countDownFrom == -1) {
        emit playMusic();
        QMetaObject::invokeMethod(this->countDown, "stop", Qt::QueuedConnection);
        this->startTimers();
    }
}

// Change the text of the top right timer
void GameTime::displayGameTime() {

    this->gameTimeFrom--;
    int minutes = this->gameTimeFrom / 60;
    int seconds = this->gameTimeFrom % 60;

    // Format time like this: 3:43, 1:20, 0:12...
    QString text = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
    this->gameTimeText->setPlainText(text);

    // When the time ends, stop the timers and show winner
    if (this->gameTimeFrom == 0) {
        this->stopTimers();
        emit gameEnd();
    }
}

// Show the winner at the end of the game
void GameTime::showWinner(QString winnerName) {
    this->countDownText->setScale(10 * this->scale);
    this->countDownText->setPlainText(winnerName);
    this->countDownText->setPos(centerX - this->countDownText->boundingRect().width() * (10 * scale) / 2.0,
                                centerY - this->countDownText->boundingRect().height() * (10 * scale) / 2.0);
}

GameTime::GameTime(float centerX, float rightX, float centerY, float scale, QObject *parent) : QObject(parent) {

    this->gameThread = new QThread(this);

    this->countDown = new QTimer(this);
    this->gameTime = new QTimer(this);
    this->gameLoop = new QTimer();

    // This is IMPORTANT! Without this it doesn't run on the FPS you want
    this->countDown->setTimerType(Qt::PreciseTimer);
    this->gameTime->setTimerType(Qt::PreciseTimer);
    this->gameLoop->setTimerType(Qt::PreciseTimer);

    // Move timers to threads
    this->gameLoop->moveToThread(this->gameThread);

    // Connect them to the relevant functions
    connect(this->countDown, &QTimer::timeout, this, &GameTime::displayCountDown);
    connect(this->gameTime, &QTimer::timeout, this, &GameTime::displayGameTime);
    connect(this->gameLoop, &QTimer::timeout, qobject_cast<MainWindow*>(this->parent()), &MainWindow::gameLogic);
    connect(this, &GameTime::gameEnd, qobject_cast<MainWindow*>(this->parent()), &MainWindow::gameFinished);
    connect(this, &GameTime::playMusic, qobject_cast<MainWindow*>(this->parent()), &MainWindow::playMusic);

    // Text
    this->countDownText = new QGraphicsTextItem("3");
    this->countDownText->setDefaultTextColor(QColor(Qt::white));
    this->countDownText->setEnabled(false);
    this->countDownText->setScale(15 * scale);

    this->gameTimeText = new QGraphicsTextItem("2:00");
    this->gameTimeText->setDefaultTextColor(QColor(Qt::white));
    this->gameTimeText->setEnabled(false);
    this->gameTimeText->setScale(2 * scale);

    this->gameThread->start();

    this->centerX = centerX;
    this->centerY = centerY;
    this->scale = scale;

    this->countDownText->setPos(centerX - this->countDownText->boundingRect().width() * (15 * scale) / 2.0,
                                centerY - this->countDownText->boundingRect().height() * (15 * scale) / 2.0);
    this->gameTimeText->setPos(rightX - this->gameTimeText->boundingRect().width() * (2 * scale) - (45.5 * scale),
                               37 * scale);

}

GameTime::~GameTime() {
    quitThread();

    delete countDown;
    delete gameTime;
    delete gameLoop;
    delete gameThread;
    delete countDownText;
    delete gameTimeText;

    countDown = nullptr;
    gameTime = nullptr;
    gameLoop = nullptr;
    gameThread = nullptr;
    countDownText = nullptr;
    gameTimeText = nullptr;
}
