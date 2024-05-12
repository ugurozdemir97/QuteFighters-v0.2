#ifndef GAMEVIEW_H
#define GAMEVIEW_H
#include <QGraphicsView>

// This class is the QGraphicsView of our game, it will handle resizing items, zoom in - out and shaking the camera in some scenarios
class GameView : public QGraphicsView {
    public:
        GameView(QWidget* parent = nullptr);
        QRectF visiblePortion;  // Used to setPos() items in the scene

    protected:
        void resizeEvent(QResizeEvent* event) override;
};

#endif // GAMEVIEW_H
