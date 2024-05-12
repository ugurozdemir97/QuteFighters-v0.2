#include "gamescene.h"
#include <QGraphicsTextItem>

GameScene::GameScene(QObject *parent) : QGraphicsScene{parent} {
    this->setItemIndexMethod(QGraphicsScene::NoIndex);  // To improve performance
}
