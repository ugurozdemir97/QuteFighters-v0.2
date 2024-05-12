#include "healthbar.h"
#include <QPainter>

// Virtual methods of QGraphicsItem
QRectF HealthBar::boundingRect() const {return QRectF(0, 0, pixmap.width(), pixmap.height());}
void HealthBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawPixmap(0, 0, pixmap);
}

// When a player got hit, decrease the health bar's width.
void HealthBar::shortenHealth(int currentHealth) {

    // Current health can't be negative
    currentHealth = currentHealth < 0 ? 0 : currentHealth;

    // If the maximum health is this wide, then the width of new health is:
    double newWidth = currentHealth * 524 / this->maxHealth;
    newWidth = newWidth < 0 ? 0 : newWidth;  // Prevent making it negative width

    // To wich direction the bar will decrease the width.
    // // If it's the left health bar the health should decrease from right, if it's the right one, it should decrease from left
    if (this->direction) this->greenBar->setRect(95, 12, newWidth, 29);
    else {
        double newX = 524 - newWidth + 5;
        this->greenBar->setRect(newX, 12, newWidth, 29);
    }

    this->healthValue->setPlainText(QString::number(this->maxHealth) + "/" + QString::number(currentHealth).rightJustified(3, '0'));

}

// Initialize health bars
HealthBar::HealthBar(const QPixmap &pixmap, const QPixmap &avatarChar, int health, bool direction, QString name,
                     float scale, float left, float right, QGraphicsItem *parent) :
    QGraphicsItem(parent), pixmap(pixmap) {

    this->maxHealth = health;    // Total health of the character, used to calculate the width of this->health
    this->direction = direction; // Is the health bar on the right or left

    this->greenBar = new QGraphicsRectItem(this);
    this->avatar = new QGraphicsPixmapItem(avatarChar.scaledToWidth(100, Qt::SmoothTransformation), this);
    this->healthValue = new QGraphicsTextItem(QString::number(health) + "/" + QString::number(health), this);
    this->playerName = new QGraphicsTextItem(name, this);

    // Font settings
    QColor whiteColor = QColor(Qt::white);
    QFont font;
    font.setBold(true);

    this->healthValue->setDefaultTextColor(QColor(Qt::white));
    this->playerName->setDefaultTextColor(QColor(Qt::white));
    this->healthValue->setFont(font);
    this->playerName->setFont(font);

    // Scale items properly
    this->setScale(scale);
    this->healthValue->setScale(2);
    this->playerName->setScale(1.8);

    // Set the position
    if (this->direction) {
        this->setPos(left + (135 * scale), 50 * scale);
        this->avatar->setPos(-5, -40);
        this->healthValue->setPos(250, 1);
        this->playerName->setPos(95, 42);
        this->greenBar->setRect(95, 12, 524, 29);
    } else {
        this->setPos(right - (135 * scale) - (this->pixmap.width() * scale), 50 * scale);
        this->avatar->setPos(this->pixmap.width() + (5) - (this->avatar->pixmap().width()), -40);
        this->healthValue->setPos(260, 1);
        this->playerName->setPos(255, 42);
        this->greenBar->setRect(5, 12, 524, 29);
    }

    // Add gradient to the green health bar and remove the border
    QLinearGradient gradient(QPointF(0, this->greenBar->rect().bottom()), QPointF(0, this->greenBar->rect().top()));
    gradient.setColorAt(0, Qt::darkGreen);
    gradient.setColorAt(1, Qt::green);
    this->greenBar->setBrush(gradient);
    this->greenBar->setPen(QPen(Qt::transparent));

}

HealthBar::~HealthBar() {
    delete avatar;
    delete greenBar;
    delete healthValue;
    delete playerName;

    avatar = nullptr;
    greenBar = nullptr;
    healthValue = nullptr;
    playerName = nullptr;
}
