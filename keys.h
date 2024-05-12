#ifndef KEYS_H
#define KEYS_H

#include <QDialog>
#include "globals.h"

namespace Ui {
    class Keys;
}

// This class is responsible from changing and viewing key bindings
class Keys : public QDialog {
    Q_OBJECT

    public:
        explicit Keys(QMap<KeyActions, int> &player1Ref, QMap<KeyActions, int> &player2Ref, QWidget *parent = nullptr);
        ~Keys();

    private:
        Ui::Keys *ui;

        void placeKeys(); // Place the current key bindings
        void focusText(); // Make a button ready to change key binding
        void saveKeys();  // Change the original key bindings
        bool checkedKey = false;  // If this is false prevent keyPressEvent

        // This will be used to pair buttons with their actions
        QMap<QString, KeyActions> buttonKeyPairs = {
            {"player1edit1", Left},
            {"player1edit2", Right},
            {"player1edit3", Crouch},
            {"player1edit4", Jump},
            {"player1edit5", LA},
            {"player1edit6", HA},
            {"player1edit7", Block},
            {"player2edit1", Left},
            {"player2edit2", Right},
            {"player2edit3", Crouch},
            {"player2edit4", Jump},
            {"player2edit5", LA},
            {"player2edit6", HA},
            {"player2edit7", Block}
        };

        // These are copies of the key bindings. We will work on these until the user clicks "Okay"
        QMap<KeyActions, int> player1copy;
        QMap<KeyActions, int> player2copy;

        // These are the original key bindings. These will be changed with copies if clicked "Okay"
        QMap<KeyActions, int> &player1;
        QMap<KeyActions, int> &player2;

    protected:
        void keyPressEvent(QKeyEvent *event);
};

#endif // KEYS_H
