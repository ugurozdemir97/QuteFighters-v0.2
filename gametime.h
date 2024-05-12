#ifndef GAMETIME_H
#define GAMETIME_H
#include <QGraphicsTextItem>
#include <QCloseEvent>
#include <QThread>
#include <QObject>
#include <QTimer>

// This class handles the timers and threads in the game
class GameTime : public QObject {
    Q_OBJECT

    public:
        GameTime(float centerX, float rightX, float centerY, float scale, QObject *parent = nullptr);
        ~GameTime();

        QGraphicsTextItem *countDownText = nullptr;  // Count down before the game and show winner after the game
        QGraphicsTextItem *gameTimeText = nullptr;   // Timer on the top right corner

        void showWinner(QString winnerName);  // Show the winner at the end of the game
        int countDownFrom = 3;

    public slots:
        void startTimers();  // Start and stop all timers
        void stopTimers();

    private:
        QThread *gameThread = nullptr;  // For the gameLoop timer to run with a better performance
        QTimer *countDown = nullptr;    // 3, 2, 1, FIGHT!
        QTimer *gameTime = nullptr;     // X minutes of game time
        QTimer *gameLoop = nullptr;     // 30 (?) FPS interval timer

        void displayCountDown();  // Change the text of the timers
        void displayGameTime();
        void quitThread(); // Just in case the game window is closed while the thread is running

        int gameTimeFrom = 120;
        const int FPS = 33;

        float centerX = 0.0f;  // Used to center text
        float centerY = 0.0f;
        float scale = 0.0f;

    signals:
        void gameEnd();   // If the timer reaches 0, emit gameEnd signal, MainWindow will return who won the game info
        void playMusic(); // Play a different music
};

#endif // GAMETIME_H
