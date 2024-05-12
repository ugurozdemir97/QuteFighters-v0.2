#include "background.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>


// Virtual methods of QGraphicsItem must be included.
QRectF Background::boundingRect() const {return QRectF(0, 0, pixmap.width(), pixmap.height());}

// Clip the background
void Background::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // This will allow the painter to paint ONLY the visible portion of the scenes. This will immprove performance
    QRectF clipRect = option->exposedRect;
    painter->setClipRect(clipRect);
    painter->drawPixmap(clipRect, pixmap, clipRect);
}

// When this function is called, the scene will move to the left or right
void Background::moveScene(bool direction, float speed, float skew) {

    // How fast the scene will move or how much it will skew depends on the players' speed.
    // So those calculations will be done in the Player class.

    // Skew if the layer should be skewed, If direction == true, move right. The skew will create a 3D parallax effect
    if (skew) {
        if (direction) {
            this->transform.shear(qTan(qDegreesToRadians(1) * skew), 0);
            this->setTransform(this->transform);
            this->setPos(this->x() - speed, this->y());
        } else {
            this->transform.shear(qTan(qDegreesToRadians(-1) * skew), 0);
            this->setTransform(this->transform);
            this->setPos(this->x() + speed, this->y());
        }

    } else {
        direction ? this->setPos(this->x() + speed, this->y()) : this->setPos(this->x() - speed, this->y());
    }

}

// Initialize Background class
Background::Background(const QPixmap &pixmap, double scale, QGraphicsItem *parent) : QGraphicsItem(parent), pixmap(pixmap) {
    this->setScale(scale);
    this->setEnabled(false);
}
