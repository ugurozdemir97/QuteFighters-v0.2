#include "gameview.h"
#include <QResizeEvent>
#include <QScrollBar>
#include <QDateTime>

void GameView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    qreal newScale = static_cast<qreal>(event->size().height()) / static_cast<qreal>(this->sceneRect().height());
    this->setTransform(QTransform::fromScale(newScale, newScale));
    this->visiblePortion = this->mapToScene(this->viewport()->geometry()).boundingRect();  // Used to setPos() items in the scene
}

GameView::GameView(QWidget* parent): QGraphicsView(parent) {
    this->setEnabled(false);
    this->setInteractive(false);
    this->setFocusPolicy(Qt::NoFocus);
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);  // Scroll to the same point after resizing. IMPORTANT LINE !!!
}
