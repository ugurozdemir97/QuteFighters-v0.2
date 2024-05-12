#ifndef GAMESCENE_H
#define GAMESCENE_H
#include <QGraphicsScene>

// This class is our game scene
class GameScene : public QGraphicsScene {
    public:
        explicit GameScene(QObject *parent = nullptr);
};

#endif // GAMESCENE_H
