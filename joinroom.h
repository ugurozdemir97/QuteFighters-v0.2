#ifndef JOINROOM_H
#define JOINROOM_H

#include <QDialog>

namespace Ui {
    class JoinRoom;
}

// This class is responsible from join modal
class JoinRoom : public QDialog {
    Q_OBJECT

    public:
        explicit JoinRoom(QWidget *parent = nullptr);
        ~JoinRoom();

    private:
        Ui::JoinRoom *ui;
        void isValid();  // Checks if the room number is valid

    signals:
        void hideOverlay();  // Emit hide overlay signal to the MainWindow

    protected:
        void keyPressEvent(QKeyEvent *event); // For closing the modal with ESC

};

#endif // JOINROOM_H
