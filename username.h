#ifndef USERNAME_H
#define USERNAME_H
#include <QDialog>

namespace Ui {
    class Username;
}

// This is our username modal, you can change your username here
class Username : public QDialog{
    Q_OBJECT

    public:
        explicit Username(QString &name, QWidget *parent = nullptr);
        ~Username();

    private:
        Ui::Username *ui;

        QString &username;
        void saveUsername();  // Change the username

    protected:
        void keyPressEvent(QKeyEvent *event); // For closing the modal with ESC

    signals:
        void hideOverlay();  // Emit hide overlay signal to the MainWindow
};

#endif // USERNAME_H
