#ifndef SELECTION_H
#define SELECTION_H
#include <QGraphicsScene>
#include <QJsonArray>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

// This class manages the character selection screen. It can allow users to select/decelect characters and display the selected
// characters. When both users select characters and click play, the selected characters will be created.
class Selection : public QGraphicsScene {
    Q_OBJECT
    public:
        explicit Selection(float scale, int roomNumber, QObject *parent = nullptr);
        ~Selection();

        // If local game, both will be true, if it's online eighter one of them will be true or both of them will be false
        void createSelector(bool player1, bool player2);  // Create selectors
        void placePlayer1(QString playerName, int num);  // Place player info; selected character, playername, character info etc.
        void placePlayer2(QString playerName, int num);

        int currentPlayerNumber = 0; // Used to track which player we are in an online game.

        bool readyPlayer1 = false;  // If the users selected characters and ready to play
        bool readyPlayer2 = false;

        QJsonArray characters;  // All information about the characters

        int player1Selects = 0; // Index of the selected character, use this to get info of the character.
        int player2Selects = 0;

        // 0, 1, 2, 3 -> Top, Right, Bottom, Left respectively
        void moveSelector(int direction, QGraphicsPixmapItem &selector, int &playerSelect, bool playerReady);

        void displayCharacter1();  // Display the selected characters and change them when selectors are moving
        void displayCharacter2();

        void selectCharacter1();   // Select character when light attack button pressed
        void selectCharacter2();

        void deselectCharacter1(); // Deselect character when heavy attack button pressed
        void deselectCharacter2();

        void deleteSelector();     // If we become a spectator, delete our selector
        void deleteDisplay(int number);

        QGraphicsPixmapItem *selector1 = nullptr;  // Little arrows and a circle to show current selected item
        QGraphicsPixmapItem *selector2 = nullptr;

        QGraphicsTextItem *player1Name = nullptr;
        QGraphicsTextItem *player2Name = nullptr;

    private:
        QVector<QMap<QString, int>> selectorPos;

        QGraphicsPixmapItem *selected1 = nullptr;  // Selected character display of player 1 and 2
        QGraphicsPixmapItem *selected2 = nullptr;

        QGraphicsPixmapItem *background = nullptr; // Background image

        QGraphicsTextItem *character1Name = nullptr;  // Texts next to the displayed characters
        QGraphicsTextItem *character2Name = nullptr;
        QGraphicsTextItem *details1Speed = nullptr;
        QGraphicsTextItem *details2Speed = nullptr;
        QGraphicsTextItem *details1Health = nullptr;
        QGraphicsTextItem *details2Health = nullptr;
        QGraphicsTextItem *details1Strength = nullptr;
        QGraphicsTextItem *details2Strength = nullptr;
        QGraphicsTextItem *roomNumber = nullptr;

        void createText(QGraphicsTextItem **item, QString text, QFont font, int fontSize, int YPos);
        void addCharacters();  // Add all the characters' avatars to the selection screen. It will pull the data from a JSON file.

        float scale = 0.0f;

    signals:
        void charactersReady(bool ready);  // Send this signal when both users select characters

};

#endif // SELECTION_H
