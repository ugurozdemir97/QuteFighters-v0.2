#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "globals.h"
#include <QUdpSocket>
#include <QPushButton>
#include <QListWidget>
#include "selection.h"
#include "selectionview.h"
#include "gameview.h"
#include "gamescene.h"
#include "background.h"
#include "gametime.h"
#include "healthbar.h"
#include "player.h"
#include "gamemodal.h"
#include <QMediaPlayer>
#include <QSoundEffect>

QT_BEGIN_NAMESPACE
    namespace Ui {class MainWindow;}
QT_END_NAMESPACE

// This is our main window and it will connect and manage all classes together

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        // UI and Scaling
        double scale = 0.0;         // This is important for scaling widgets and scenes
        void resizeWidgets();       // This function is called in resizeEvent to resize the widgets
        QWidget *overlay = nullptr; // This will be used as an overlay when you create modals or dialogs
        QPushButton *exitRoomButton = nullptr;  // Go back button in the waiting room
        QPushButton *startButton = nullptr;     // Start the game button
        QPushButton *openChatButton = nullptr;  // Open and close chat area
        QListWidget *chatArea = nullptr;        // Chat area
        QLineEdit *chatLine = nullptr;          // Sending message line

        // Game Timers and Threads
        GameTime *gameTime = nullptr;  // Handles displaying time and managing threads

        // QGraphics
        SelectionView *selectionView = nullptr; // This class is the view of Selection class and responsible from resizing items
        Selection *selection = nullptr;         // This is a class responsible from character selection
        GameView *gameView = nullptr;           // This class responsible from game zoom in-out, camera shaking and resizing
        GameScene *gameScene = nullptr;         // This is the scene that will have all items in the game

        // Game items
        Background *scene1 = nullptr;  // The background is made up from 3 layers to create a 3D parallax effect
        Background *scene2 = nullptr;
        Background *scene3 = nullptr;
        HealthBar *healthBar1 = nullptr;  // Health bars
        HealthBar *healthBar2 = nullptr;
        Player *player1 = nullptr;  // Players
        Player *player2 = nullptr;

        // Game Modal that opens when you press ESC
        GameModal *gameModal = nullptr;

        // Music and Sound
        QSoundEffect *soundPlayer1 = nullptr;  // Ideal for short sounds like clicking, punching effects
        QSoundEffect *soundPlayer2 = nullptr;  // Ideal for short sounds like clicking, punching effects
        QMediaPlayer *musicPlayer = nullptr;  // Our music player which will play random musics from a folder
        QAudioOutput *musicOutput = nullptr;  // I guess this is necessary to play the sound
        QStringList menuMusicList;  // Musics played in main menu
        QStringList gameMusicList;  // Musics played while fighting

        // Key bindings of the player 1 and player 2, these will be used in keyPressEvent amd keyReleaseEvent.
        // They can be changed through settings
        QMap<KeyActions, int> player1Keys = {
            {Left, 65},
            {Right, 68},
            {Crouch, 83},
            {Jump, 87},
            {Block, 89},
            {LA, 71},
            {HA, 74}
        };
        QMap<KeyActions, int> player2Keys = {
            {Left, 16777234},
            {Right, 16777236},
            {Crouch, 16777237},
            {Jump, 16777235},
            {Block, 80},
            {LA, 73},
            {HA, 76}
        };

        // Reverse map of key bindings, used to apply methods
        QMap<int, KeyActions> player1Methods = {};
        QMap<int, KeyActions> player2Methods = {};

        // This is for handling multiple key press events. For example crouch block, side light attack etc.
        QSet<int> pressedKeys;
        QSet<int> opponentPressedKeys;  // Store opponent's keys in an online game

        // Music and Sound
        float musicVolume = 100;  // These can be changed through soundBar
        float soundVolume = 100;

        // Connection and Online Gameplay
        QString username = "";  // This doens't have to be unique since there is no database, users can name themselves however they want
        const QString id = QUuid::createUuid().toString(); // This is for online gameplay. This ID will be used to identify clients in the server
        QUdpSocket *socket = nullptr;  // Our socket to connect to the server, UDP sockets are faster than TCP
        int currentRoom = 0;           // If we are in a room this will be a 6 digit room number
        bool playerStatus = false;     // If we are a spectator or not. If we are not, this will be true
        bool connectedServer = false;  // If we are connected to a server or not

        int FPStest = 1;

        void movePlayers(Player &one, Player &two);  // Do the calculations to move player objects
        void checkHitboxCollision(Player &one, Player &two, HealthBar &healthBar);  // Check if the hitbox touches the hurtbox
        //void moveShadows();                          // Make shadows follow the players

        int maxDistance = 0;      // How far away can players be
        float left = 0.0f;        // Coordinates of the visible portion of the QGraphicsScene
        float right = 0.0f;
        float sceneLimitLeft = 0.0f;   // How left or right the player can go max
        float sceneLimitRight = 0.0f;

    public slots:
        void showOverlay();  // Show and hide the overlay
        void hideOverlay();

        void updateUsername();  // Update the username

        void activateStartButton(bool ready);  // When both players select characters activate the start button

        void changeKeyBindings();  // Change player 1 and player 2 key bindings

        void joinRoom(int roomNumber);    // Join a room

        void gameLogic();    // Game loop
        void gameFinished(); // Show the winner, a couple of seconds later turn back to the waiting room and delete game related stuff

        void changeMusicVolume(float volume);  // These methods are called from the game modal
        void changeSoundVolume(float volume);
        void playSound1(QString soundPath);   // Play the sound of player moves, this is called from the Player class
        void playSound2(QString soundPath);
        void playMusic();        // Play the music according to the page we are currently in

        void gameModalClosed();  // This is called from gameModal. Continue the timers if they are stopped.
        void exitGame();         // Delete game scene and its items, then turn back to the waiting room

    private slots:

        // Navigate between pages
        void aboutPage();
        void howToPage();
        void backToMain();
        void playPage();

        // Show hide
        void showMusicBar();  // Show the music bar
        void showKeyModal();  // Show key bindings settings
        void showJoinModal(); // Show join room modal
        void showGameModal(); // Show game modal if you press ESC in game
        void showUsernameModal();  // Show username modal
        void hidePlayPage();  // Hide Create, Join, Local buttons
        void showChatArea();  // Show chat area

        // Waiting room UI, must be created manually because they will be on top of a QGraphicsView
        void waitingRoomUI(); // Create the UI elements like buttons and chat area for the waiting room

        // Change window mode
        void changeScreenMode();

        // Change values
        void changeVolume();  // Change music or sound volume

        // Server related methods
        void sendKeyAction(KeyActions action, bool press); // Send the key inputs to the opponent (Which key and press or release)
        void getMessage();      // Get message from server (Opponent moves, message from friends, connection info)
        void sendMessage();     // Send message to your friend
        void connection();      // Connect to the server
        void connected();       // Inform user about connection
        void createRoom();      // Create a room
        void exitRoom();        // Exit the room
        void leaveGame();       // Leave player status
        void joinGame();        // Get player status
        void characterChange(); // Select or deselect character
        void roleChange();      // Inform all players and spectators about a change
        void sendGameStart();   // Inform players about that the game is started
        void movePlayerOnline(Player &player, bool press, int key, QSet<int> &keys); // Move the opponent's character
        void movePlayer(Player &player, bool press, int key, QMap<KeyActions, int> keys, QMap<int, KeyActions> methods);
        void waitingRoom(bool player1, bool player2); // Open waiting room, player1 and player2 determines if  we create a selector
        void exitWaitingRoom();                       // Destroy everything related to waiting room to free up memory
        void gameStart();                             // Create game page

        // Music related
        void stateOfMusic();     // Change the music if it ends
        void setUpMusic();       // Create audio player to play music

    protected:
        void closeEvent(QCloseEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void keyPressEvent(QKeyEvent *event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
        //void mousePressEvent(QMouseEvent *event) override;  // For game development

};

#endif // MAINWINDOW_H
