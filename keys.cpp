#include "keys.h"
#include "ui_keys.h"
#include <QKeyEvent>
#include "mainwindow.h"

// If Okay is clicked, change the original key bindings with the copies created here.
void Keys::saveKeys() {
    this->player1 = this->player1copy;
    this->player2 = this->player2copy;
}

// Place the new value, change the copy of key bindings
void Keys::keyPressEvent(QKeyEvent *event) {

    if (this->checkedKey) {
        // Look for all buttons,
        QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();
        for (QPushButton *button : allButtons) {
            if (button->isChecked()) {
                button->setChecked(false);
                this->checkedKey = false;
                button->setText(QKeySequence(event->key()).toString());

                if (button->objectName().contains("player1")) this->player1copy[buttonKeyPairs[button->objectName()]] = event->key();
                else this->player2copy[buttonKeyPairs[button->objectName()]] = event->key();

            // If a button is not checked and has the same key binding, remove that key binding from that button
            } else {
                if (button->text() == QKeySequence(event->key()).toString()) {
                    button->setText("");
                    if (button->objectName().contains("player1")) this->player1copy[buttonKeyPairs[button->objectName()]] = 0;
                    else this->player2copy[buttonKeyPairs[button->objectName()]] = 0;

                }
            }
        }
    }
}

// Make the button ready to change
void Keys::focusText() {

    // Uncheck all the buttons
    QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();
    for (QPushButton *button : allButtons) {
        if (button->isChecked()) button->setChecked(false);
    }

    // Check the button I clicked and set text to none. In keyPressEvents I will check for a checked button and change its text
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    clickedButton->setText("");
    clickedButton->setChecked(true);
    this->checkedKey = true;
}

// A for loop for all buttons, if the button has player1 or player2 in it, use buttonKeyPairs to place the correct text
void Keys::placeKeys() {
    QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();
    for (QPushButton *button : allButtons) {
        if (button->objectName().contains("player1")) {
            button->setText(QKeySequence(this->player1copy[buttonKeyPairs[button->objectName()]]).toString());
        } else if (button->objectName().contains("player2")) {
            button->setText(QKeySequence(this->player2copy[buttonKeyPairs[button->objectName()]]).toString());
        }
    }
}

// Initialize Keys dialog
Keys::Keys(QMap<KeyActions, int> &player1Ref, QMap<KeyActions, int> &player2Ref, QWidget *parent) :
    QDialog(parent), ui(new Ui::Keys), player1(player1Ref), player2(player2Ref)  {
    ui->setupUi(this);

    // These will make the dialog look like a modal and make sure the modal will be deleted after it's closed
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setModal(true);

    this->player1copy = player1;  // These are copies of the original values
    this->player2copy = player2;

    this->placeKeys();  // Show the current key bindings

    // Clicking a button will focus on it and make it ready to change
    connect(ui->player1edit1, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit2, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit3, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit4, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit5, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit6, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player1edit7, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit1, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit2, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit3, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit4, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit5, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit6, &QPushButton::clicked, this, &Keys::focusText);
    connect(ui->player2edit7, &QPushButton::clicked, this, &Keys::focusText);

    // Close the dialog. If "Okay" clicked change the original key bindings
    connect(ui->okayButton, &QPushButton::clicked, this, &Keys::saveKeys);
    connect(ui->okayButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::changeKeyBindings);
    connect(ui->okayButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->okayButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->cancelButton, &QPushButton::clicked, qobject_cast<MainWindow*>(this->parent()), &MainWindow::hideOverlay);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

}

Keys::~Keys() {delete ui;}
