#ifndef HEALTHBAR_H
#define HEALTHBAR_H
#include <QGraphicsItem>

// This class handles the health bars of players
class HealthBar : public QGraphicsItem {
    public:
        HealthBar(const QPixmap &pixmap, const QPixmap &avatar, int health, bool direction, QString name,
                  float scale, float left, float right, QGraphicsItem *parent = nullptr);
        ~HealthBar();

        void shortenHealth(int currentHealth);  // Decrease health
        int maxHealth = 0;       // The maximum/total health the character has

    private:

        // Overriding virtual methods is necessary. We don't need to change these methods
        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        bool direction = false;  // If it is the left health bar or right health bar

        QGraphicsPixmapItem *avatar = nullptr;     // Selected character's face
        QGraphicsRectItem *greenBar = nullptr;     // The green bar that shows how much health left
        QGraphicsTextItem *healthValue = nullptr;  // Health in numbers
        QGraphicsTextItem *playerName = nullptr;   // Player's name

        QPixmap pixmap;  // The health bar, main item
};

#endif // HEALTHBAR_H
