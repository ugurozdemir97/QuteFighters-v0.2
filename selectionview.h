#ifndef SELECTIONVIEW_H
#define SELECTIONVIEW_H

#include <QGraphicsView>

// This class handles resizing items in the selection screen
class SelectionView : public QGraphicsView {
    public:
        SelectionView(QWidget* parent = nullptr);

    protected:
        void resizeEvent(QResizeEvent* event) override;
};

#endif // SELECTIONVIEW_H
