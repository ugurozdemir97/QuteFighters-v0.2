#include "selection.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QGraphicsPixmapItem>
#include <QCursor>
#include <QKeyEvent>
#include <QFontDatabase>

// If role changed or we become a spectator, delete the character info
void Selection::deleteDisplay(int number) {
    if (number == 1 && this->player1Name != nullptr) {
        delete this->selected1;
        delete this->player1Name;
        delete this->character1Name;
        delete this->details1Speed;
        delete this->details1Health;
        delete this->details1Strength;
        this->selected1 = nullptr;
        this->player1Name = nullptr;
        this->character1Name = nullptr;
        this->details1Speed = nullptr;
        this->details1Health = nullptr;
        this->details1Strength = nullptr;
        this->readyPlayer1 = false;
        this->player1Selects = 0;
    } else if (number == 2 && this->player2Name != nullptr) {
        delete this->selected2;
        delete this->player2Name;
        delete this->character2Name;
        delete this->details2Speed;
        delete this->details2Health;
        delete this->details2Strength;
        this->selected2 = nullptr;
        this->player2Name = nullptr;
        this->character2Name = nullptr;
        this->details2Speed = nullptr;
        this->details2Health = nullptr;
        this->details2Strength = nullptr;
        this->readyPlayer2 = false;
        this->player2Selects = 0;
    }
}

// If we become a spectator, delete the selector
void Selection::deleteSelector() {
    delete this->selector1;
    this->selector1 = nullptr;
    emit charactersReady(false);
}

// Select a character
void Selection::selectCharacter1() {
    this->selected1->setOpacity(1);
    this->character1Name->setOpacity(1);
    this->details1Speed->setOpacity(1);
    this->details1Health->setOpacity(1);
    this->details1Strength->setOpacity(1);
    this->readyPlayer1 = true;
    // If both users are ready, activate the "Play" button. Not in online
    if (this->readyPlayer2 == true && this->roomNumber == nullptr) emit charactersReady(true);
}

void Selection::selectCharacter2() {
    this->selected2->setOpacity(1);
    this->character2Name->setOpacity(1);
    this->details2Speed->setOpacity(1);
    this->details2Health->setOpacity(1);
    this->details2Strength->setOpacity(1);
    this->readyPlayer2 = true;
    if (this->readyPlayer1 == true && this->roomNumber == nullptr) emit charactersReady(true);
}

// Deselect the character
void Selection::deselectCharacter1() {
    this->selected1->setOpacity(0.4);
    this->character1Name->setOpacity(0.4);
    this->details1Speed->setOpacity(0.4);
    this->details1Health->setOpacity(0.4);
    this->details1Strength->setOpacity(0.4);
    this->readyPlayer1 = false;
    emit charactersReady(false);  // Deactivate the "Play" button when someone deselects
}

void Selection::deselectCharacter2() {
    this->selected2->setOpacity(0.4);
    this->character2Name->setOpacity(0.4);
    this->details2Speed->setOpacity(0.4);
    this->details2Health->setOpacity(0.4);
    this->details2Strength->setOpacity(0.4);
    this->readyPlayer2 = false;
    emit charactersReady(false);
}

void Selection::displayCharacter1() {
    // Change the displayed character as the selector moves
    if (this->readyPlayer1) return;
    QPixmap newPixmap = QPixmap(this->characters[player1Selects].toObject()["waiting"].toString())
                            .scaledToWidth((400 * this->scale), Qt::SmoothTransformation)
                            .transformed(QTransform().scale(-1, 1));
    this->selected1->setPixmap(newPixmap);
    this->selected1->setOpacity(0.4);

    this->character1Name->setPlainText(this->characters[player1Selects].toObject()["name"].toString());
    this->details1Speed->setPlainText("Speed         :  " + QString::number(this->characters[player1Selects].toObject()["speed"].toInt()));
    this->details1Health->setPlainText("Health       :  " + QString::number(this->characters[player1Selects].toObject()["health"].toInt()));
    this->details1Strength->setPlainText("Strength  :  " + QString::number(this->characters[player1Selects].toObject()["strength"].toInt()));
}

void Selection::displayCharacter2() {
    if (this->readyPlayer2) return;
    QPixmap newPixmap = QPixmap(this->characters[player2Selects].toObject()["waiting"].toString())
                            .scaledToWidth((400 * this->scale), Qt::SmoothTransformation);
    this->selected2->setPixmap(newPixmap);
    this->selected2->setOpacity(0.4);

    this->character2Name->setPlainText(this->characters[player2Selects].toObject()["name"].toString());
    this->details2Speed->setPlainText("Speed         :  " + QString::number(this->characters[player2Selects].toObject()["speed"].toInt()));
    this->details2Health->setPlainText("Health       :  " + QString::number(this->characters[player2Selects].toObject()["health"].toInt()));
    this->details2Strength->setPlainText("Strength  :  " + QString::number(this->characters[player2Selects].toObject()["strength"].toInt()));

    this->character2Name->setX((1920 * this->scale) - (400 * this->scale + 50 + this->character2Name->boundingRect().width()));
    this->details2Speed->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Speed->boundingRect().width()));
    this->details2Health->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Health->boundingRect().width()));
    this->details2Strength->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Strength->boundingRect().width()));
}

// moveSelector1 and moveSelector2 repeats a lot. I can rewrite these two methods in a different way sometime
// These will move the selectors
void Selection::moveSelector(int direction, QGraphicsPixmapItem &selector, int &playerSelects, bool playerReady) {

    if (playerReady) return;

    // 0, 1, 2, 3 -> Top, Right, Bottom, Left respectively
    switch (direction) {
        case 0:
            // If you can go up safely then go up
            if (0 <= playerSelects - 9) playerSelects -= 9;
            // Otherwise check if you can go to down
            else {
                // If you can't go to the bottom row, go one above
                if ((floor(characters.size() / 9) * 9) + playerSelects >= characters.size())
                    playerSelects = ((floor(characters.size() / 9) - 1) * 9) + playerSelects;
                // Go to the bottom row
                else playerSelects = (floor(characters.size() / 9) * 9) + playerSelects;
            };
            break;
        case 1:  // Just add one, if you are at the end turn back to zero
            if (characters.size() > playerSelects + 1) playerSelects += 1;
            else playerSelects = 0;
            break;
        case 2:
            // If you can safely go down, go down. Otherwise go to the top line
            if (characters.size() > playerSelects + 9) playerSelects += 9;
            else playerSelects = playerSelects % 9;
            break;
        case 3:
            // Same as right, substract one
            if (0 <= playerSelects - 1) playerSelects -= 1;
            else playerSelects = characters.size() - 1;
            break;
        default:
            break;
    }

    // If the selector is on the lower rows, the z index of it should increase.
    // This will make the selector on the bottom always on top of the othe selecter
    if (playerSelects > 8) selector.setZValue(floor(playerSelects / 9) + 1);
    else selector.setZValue(1);

    // Move the selector
    selector.setPos(this->selectorPos[playerSelects]["x"], this->selectorPos[playerSelects]["y"]);

    // If two selectors are on the same position, rotate selector one so that the triangle can be seen
    if (selector2) {
        if (selector1->pos() == selector2->pos()) {
            int x = this->selector1->x();
            int y = this->selector1->y();
            this->selector1->setTransform(QTransform().scale(1, -1));
            this->selector1->setPos(x, y + this->selector1->boundingRect().height());
        } else {
            if (!this->selector1->transform().isIdentity()) {
                this->selector1->setTransform(QTransform().scale(1, 1));
                this->selector1->setPos(this->selectorPos[player1Selects]["x"], this->selectorPos[player1Selects]["y"]);
            }
        }
    }

}

void Selection::placePlayer1(QString playerName, int selectedCharacter) {

    // Set up the font
    int fontId = QFontDatabase::addApplicationFont(":/Font/Karate.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(fontFamily);

    createText(&this->player1Name, playerName, font, 30, 0);
    this->player1Name->setX(400 * this->scale + 50);

    // Selected character display
    if (selectedCharacter != -1) {
        this->selected1 = new QGraphicsPixmapItem(QPixmap(this->characters[selectedCharacter].toObject()["waiting"].toString())
                                                      .scaledToWidth((400 * this->scale), Qt::SmoothTransformation)
                                                      .transformed(QTransform().scale(-1, 1)));

        this->selected1->setPos(0, ((1080 * this->scale) - this->selected1->pixmap().height()));
        this->selected1->setOpacity(0.4);  // Unless the user selects a character, display will be half transparent

        this->addItem(this->selected1);

        createText(&this->character1Name, this->characters[selectedCharacter].toObject()["name"].toString(), font, 25, 45);
        createText(&this->details1Speed,    "Speed         :  " + QString::number(this->characters[selectedCharacter].toObject()["speed"].toInt()), font, 20, 75);
        createText(&this->details1Health,   "Health       :  " + QString::number(this->characters[selectedCharacter].toObject()["health"].toInt()), font, 20, 100);
        createText(&this->details1Strength, "Strength  :  " + QString::number(this->characters[selectedCharacter].toObject()["strength"].toInt()), font, 20, 125);

        this->character1Name->setX(400 * this->scale + 50);
        this->details1Speed->setX(400 * this->scale + 50);
        this->details1Health->setX(400 * this->scale + 50);
        this->details1Strength->setX(400 * this->scale + 50);

    }

    if (this->readyPlayer1) this->selectCharacter1();

}

void Selection::placePlayer2(QString playerName, int selectedCharacter) {

    // Set up the font
    int fontId = QFontDatabase::addApplicationFont(":/Font/Karate.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(fontFamily);

    createText(&this->player2Name, playerName, font, 30, 0);
    this->player2Name->setX((1920 * this->scale) - (400 * this->scale + 50 + this->player2Name->boundingRect().width()));

    if (selectedCharacter != -1) {
        // Selected character display
        this->selected2 = new QGraphicsPixmapItem(QPixmap(this->characters[selectedCharacter].toObject()["waiting"].toString())
                                                      .scaledToWidth((400 * this->scale), Qt::SmoothTransformation));

        this->selected2->setPos((1920 * this->scale) - this->selected2->pixmap().width(), ((1080 * this->scale) - this->selected2->pixmap().height()));
        this->selected2->setOpacity(0.4);  // Unless the user selects a character, display will be half transparent

        this->addItem(this->selected2);

        createText(&this->character2Name, this->characters[selectedCharacter].toObject()["name"].toString(), font, 25, 45);
        createText(&this->details2Speed,    "Speed         :  " + QString::number(this->characters[selectedCharacter].toObject()["speed"].toInt()), font, 20, 75);
        createText(&this->details2Health,   "Health       :  " + QString::number(this->characters[selectedCharacter].toObject()["health"].toInt()), font, 20, 100);
        createText(&this->details2Strength, "Strength  :  " + QString::number(this->characters[selectedCharacter].toObject()["strength"].toInt()), font, 20, 125);

        this->character2Name->setX((1920 * this->scale) - (400 * this->scale + 50 + this->character2Name->boundingRect().width()));
        this->details2Speed->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Speed->boundingRect().width()));
        this->details2Health->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Health->boundingRect().width()));
        this->details2Strength->setX((1920 * this->scale) - (400 * this->scale + 50 + this->details2Strength->boundingRect().width()));
    }

    if (this->readyPlayer2) this->selectCharacter2();

}

// Create character selectors. If local create two of them. Otherwise just one selector
void Selection::createSelector(bool player1, bool player2) {

    // The area's width is 3/5 of the whole window's width
    double area = ((1920 / 5) * 3) * this->scale;
    double avatarSize = area / 10;

    if (player1) {
        this->selector1 = new QGraphicsPixmapItem(QPixmap(":/Images/Selection/Selector1.png")
                                                      .scaledToWidth(avatarSize, Qt::SmoothTransformation));
        this->selector1->setPos(this->selectorPos[0]["x"], this->selectorPos[0]["y"]);
        this->addItem(this->selector1);
        if (this->roomNumber == nullptr) this->placePlayer1("Player 1", 0);
    }
    if (player2) {

        this->selector2 = new QGraphicsPixmapItem(QPixmap(":/Images/Selection/Selector2.png")
                                                      .scaledToWidth(avatarSize, Qt::SmoothTransformation));
        this->selector2->setPos(this->selectorPos[0]["x"], this->selectorPos[0]["y"]);
        this->addItem(this->selector2);
        if (this->roomNumber == nullptr) this->placePlayer2("Player 2", 0);
    }
}

// Place the avatars in the screen
void Selection::addCharacters() {
    // I want to display characters in an area as a grid, there will be 9 characters in a row. For now there is only 2 characters
    // But as you add more characters it should automatically display them in the grid consist of 9 columns and x rows.

    // The area's width is 3/5 of the whole window's width
    double area = ((1920 / 5) * 3) * this->scale;
    double areaStart = ((1920 / 5) * this->scale);

    // Avatars should have a margin too so instead of 9 I will divide it to 11 to find the size of each avatar
    double avatarSize = area / 10;
    double marginRow = avatarSize / 2;
    double marginCol = (area / 10) / 9;

    for (int i = 0; i < this->characters.size(); i++) {

        QJsonObject character = this->characters[i].toObject();
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap(character["avatar"].toString())
                                                                .scaledToWidth(avatarSize, Qt::SmoothTransformation));
        item->setAcceptHoverEvents(true);
        item->setCursor(Qt::PointingHandCursor);

        // Go to the next row if its 9th, 18th, 29th...
        if (i != 0 && i % 9 == 0) {
            marginRow += avatarSize;
            marginCol = (area / 10) / 9;
        }

        item->setPos((areaStart + marginCol), marginRow);
        this->addItem(item);

        // Store the position of the avatar so that we can move the selectors without calculating the position
        QMap<QString, int> coordinates;
        coordinates["x"] = (areaStart + marginCol);
        coordinates["y"] = marginRow;
        this->selectorPos.append(coordinates);

        marginCol += (avatarSize + (area / 10) / 9);

    }
}

void Selection::createText(QGraphicsTextItem **item, QString text, QFont font, int fontSize, int YPos) {
    *item = new QGraphicsTextItem(text);
    font.setPointSize(fontSize * this->scale);
    (*item)->setFont(font);
    (*item)->setDefaultTextColor(QColor(Qt::white));
    (*item)->setY(((1080 * this->scale) - 250 * this->scale) + YPos * this->scale);
    if ((*item) != this->player1Name && (*item) != this->player2Name) (*item)->setOpacity(0.4);
    this->addItem(*item);
}

// Initialize Selection class. This will display characters and allow users to select characters
Selection::Selection(float scale, int currentRoom, QObject *parent) : QGraphicsScene{parent} {

    // Read JSON, all character data
    QFile file(":/Data/Characters.json");

    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument chars = QJsonDocument::fromJson(file.readAll());
    file.close();

    this->characters = chars.array();  // All characters
    this->scale = scale;

    this->background = new QGraphicsPixmapItem(QPixmap(":/Images/Selection/Selection.png"));
    this->background->setScale(this->scale);
    this->addItem(this->background);

    // Set up the font
    int fontId = QFontDatabase::addApplicationFont(":/Font/Karate.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(fontFamily);

    if (currentRoom != 0) {
        this->roomNumber = new QGraphicsTextItem("Room Number: " + QString::number(currentRoom));
        font.setPointSize(30  * this->scale);
        this->roomNumber->setFont(font);
        this->roomNumber->setDefaultTextColor(QColor(Qt::white));
        this->roomNumber->setPos(1920 * this->scale - ((20 * this->scale) + this->roomNumber->boundingRect().width()), 30 * this->scale);
        this->addItem(this->roomNumber);
    }

    this->addCharacters();

}

Selection::~Selection() {
    delete background;
    delete roomNumber;

    delete selector1;
    delete selected1;
    delete player1Name;
    delete character1Name;
    delete details1Speed;
    delete details1Health;
    delete details1Strength;

    delete selector2;
    delete selected2;
    delete player2Name;
    delete character2Name;
    delete details2Speed;
    delete details2Health;
    delete details2Strength;

    background = nullptr;
    roomNumber = nullptr;
    selector1 = nullptr;
    selected1 = nullptr;
    player1Name = nullptr;
    character1Name = nullptr;
    details1Speed = nullptr;
    details1Health = nullptr;
    details1Strength = nullptr;
    selector2 = nullptr;
    selected2 = nullptr;
    player2Name = nullptr;
    character2Name = nullptr;
    details2Speed = nullptr;
    details2Health = nullptr;
    details2Strength = nullptr;
}

