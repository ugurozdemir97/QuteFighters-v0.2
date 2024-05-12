#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <QGraphicsItem>

// This class handles moving and skewing the background in game as the players move
class Background : public QGraphicsItem {

    public:
        Background(const QPixmap &pixmap, double scale, QGraphicsItem *parent = nullptr);
        void moveScene(bool direction, float speed, float skew = 0);  // When this is called the scene will move and skew if necessary

    private:
        QPixmap pixmap;
        QTransform transform;  // Used to skew the ground

        // Virtual methods of QGraphicsItem must be included.
        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // BACKGROUND_H
