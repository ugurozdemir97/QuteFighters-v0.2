#include "selectionview.h"
#include <QResizeEvent>

// The screen is always in full screen but just in case, if a player somehow resize the window, it should keep the aspect ratio
void SelectionView::resizeEvent(QResizeEvent *event) {

    QGraphicsView::resizeEvent(event);
    qreal newScale = static_cast<qreal>(event->size().width()) / static_cast<qreal>(this->sceneRect().width());
    this->setTransform(QTransform::fromScale(newScale, newScale));

}

SelectionView::SelectionView(QWidget* parent): QGraphicsView(parent) {
    this->setFocusPolicy(Qt::NoFocus);  // Prevent scrolling with arrow keys
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
