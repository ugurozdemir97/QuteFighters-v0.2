#ifndef GAMEMODAL_H
#define GAMEMODAL_H
#include <QDialog>

namespace Ui {
    class GameModal;
}

// This is the modal that opens when you press ESC when you are playing
class GameModal : public QDialog {
    Q_OBJECT

    public:
        explicit GameModal(float music, float sound, QWidget *parent = nullptr);
        ~GameModal();

    private:
        Ui::GameModal *ui;

        void soundChange();
        void musicChange();

    protected:
        void keyPressEvent(QKeyEvent *event); // For closing the modal with ESC

    signals:
        void newMusicVolume(float volume);
        void newSoundVolume(float volume);
        void gameModalClosed();  // Emit start timer signal to the MainWindow

};

#endif // GAMEMODAL_H
