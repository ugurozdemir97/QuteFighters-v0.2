#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QAudioOutput>
#include <QJsonObject>
#include <QLineEdit>
#include <QThread>
#include <QTimer>
#include <QUuid>

// This is only for game development
//void MainWindow::mousePressEvent(QMouseEvent *event) {qDebug() << event->pos();}

// Switch between full screen and windowed
void MainWindow::changeScreenMode() {
    if (ui->screenButton->isChecked()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        this->resize(width, height);
        this->showFullScreen();

    } else {
        this->showNormal();
        this->resize(800, 450);
    }
}

// Create the buttons and chat area on top of a QGraphicsView for the waiting room
void MainWindow::waitingRoomUI() {

    // Back to the play page
    this->exitRoomButton = new QPushButton(ui->pages->widget(2));
    this->exitRoomButton->setStyleSheet("QPushButton {border-image: url(:/Images/UI/Go back1.png);}"
                                        "QPushButton:hover {border-image: url(:/Images/UI/Go back2.png);}");
    this->exitRoomButton->setCursor(Qt::PointingHandCursor);
    this->exitRoomButton->setFocusPolicy(Qt::NoFocus);

    // Start the game
    this->startButton = new QPushButton(ui->pages->widget(2));
    this->startButton->setStyleSheet(
        "QPushButton {border-image: url(:/Images/UI/Button.png); font-family: 'Karate'; color: rgb(218, 248, 147);}"
        "QPushButton:hover {border-image: url(:/Images/UI/ButtonHover.png);}"
        "QPushButton:disabled {border-image: url(:/Images/UI/ButtonDisabled.png); color: rgb(220, 220, 220);}"
        );
    this->startButton->setCursor(Qt::PointingHandCursor);
    this->startButton->setText("Start");
    this->startButton->setEnabled(false);
    this->startButton->setFocusPolicy(Qt::NoFocus);

    // Show hide chat area
    this->openChatButton = new QPushButton(ui->pages->widget(2));
    this->openChatButton->setCheckable(true);
    this->openChatButton->setChecked(false);
    this->openChatButton->setStyleSheet("border-image: url(:/Images/Selection/ChatBar.png);");
    this->openChatButton->setCursor(Qt::PointingHandCursor);
    this->openChatButton->setFocusPolicy(Qt::NoFocus);

    // Chat area
    this->chatArea = new QListWidget(ui->pages->widget(2));
    this->chatArea->setStyleSheet("QListWidget {border-image: url(:/Images/UI/Keys.png); padding-bottom: 24px;}");

    // Sending message
    this->chatLine = new QLineEdit(ui->pages->widget(2));
    this->chatLine->setStyleSheet("background-color: rgba(0, 0, 0, 60); border: none; color: white; padding-left: 24px;");
    this->chatLine->setPlaceholderText("Press Enter to send!");

    // Hide all, show them only in the waiting room
    this->exitRoomButton->hide();
    this->startButton->hide();
    this->openChatButton->hide();
    this->chatArea->hide();
    this->chatLine->hide();

    connect(this->openChatButton, &QPushButton::clicked, this, &MainWindow::showChatArea);
    connect(this->exitRoomButton, &QPushButton::clicked, this, &MainWindow::exitWaitingRoom);
    connect(this->startButton, &QPushButton::clicked, this, &MainWindow::sendGameStart);
}

// Initialize MainWindow
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Hide sound bar in the main menu. I brought it to back and brought music and sound buttons to front.
    // So they will be clickable after soundBar is shown
    ui->soundBar->hide();
    ui->soundBarLabel->hide();

    // Hide play buttons
    ui->localButton->hide();
    ui->joinButton->hide();
    ui->createButton->hide();
    ui->backButton2->hide();

    // QGraphicsView of character selection screen
    this->selectionView = new SelectionView(ui->pages->widget(2));
    this->selectionView->setGeometry(0, 0, this->width(), this->height());
    this->selectionView->hide();

    // Setup for the waiting room
    this->waitingRoomUI();

    // Set key bindings
    this->changeKeyBindings();

    // Set up music
    this->setUpMusic();

    // Username
    this->username = "Player" + QString::number(QRandomGenerator::global()->bounded(1000, 10000));
    ui->usernameButton->setText(this->username);

    this->socket = new QUdpSocket(this);        // Create a socket
    this->socket->bind(QHostAddress::LocalHost, 3000);  // Connect to the server

    // I want the game to be full screen
    this->showFullScreen();

    // Server related methods
    connect(this->socket, &QUdpSocket::readyRead, this, &MainWindow::getMessage);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connection);
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::createRoom);
    //connect(ui->messageSendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);

    // Connect buttons with their functions
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::backToMain);
    connect(ui->howToButton, &QPushButton::clicked, this, &MainWindow::howToPage);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::aboutPage);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::playPage);
    connect(ui->localButton, &QPushButton::clicked, this, [this](){waitingRoom(true, true);});
    connect(ui->backButton2, &QPushButton::clicked, this, &MainWindow::hidePlayPage);
    connect(ui->musicButton, &QPushButton::clicked, this, &MainWindow::showMusicBar);
    connect(ui->soundButton, &QPushButton::clicked, this, &MainWindow::showMusicBar);
    connect(ui->changeKeysButton, &QPushButton::clicked, this, &MainWindow::showKeyModal);
    connect(ui->usernameButton, &QPushButton::clicked, this, &MainWindow::showUsernameModal);
    connect(ui->joinButton, &QPushButton::clicked, this, &MainWindow::showJoinModal);
    connect(ui->soundBar, &QSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(ui->screenButton, &QPushButton::clicked, this, &MainWindow::changeScreenMode);
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // Inform server about our connection and give the server our Id
    this->connection();

}

// This function will be called from resizeEvent when the screen is setup or if a user somehow resize the window
void MainWindow::resizeWidgets() {
    struct WidgetProperties {int x, y, width, height;};  // Used in defaultProperties

    // Original size of the screen and original position and size of the widgets when I design the UI in Qt Creator
    QSize defaultSize(800, 450);
    QHash<QWidget*, WidgetProperties> defaultProperties = {
        {ui->playButton, {360, 340, 80, 80}},
        {this->startButton, {360, 340, 80, 80}},
        {ui->localButton, {360, 340, 80, 80}},
        {ui->aboutButton, {450, 345, 70, 70}},
        {ui->joinButton, {450, 345, 70, 70}},
        {ui->howToButton, {280, 345, 70, 70}},
        {ui->createButton, {280, 345, 70, 70}},
        {ui->musicButton, {366, 140, 20, 20}},
        {ui->soundButton, {416, 140, 20, 20}},
        {ui->backButton, {10, 10, 40, 40}},
        {ui->backButton2, {10, 10, 40, 40}},
        {this->exitRoomButton, {10, 10, 40, 40}},
        {ui->closeButton, {430, 98, 31, 21}},
        {ui->screenButton, {340, 100, 18, 18}},
        {ui->soundBarLabel, {260, 0, 281, 321}},
        {ui->soundBar, {276, 254, 248, 45}},
        {ui->aboutLabel1, {140, 190, 521, 31}},
        {ui->aboutLabel2, {140, 220, 521, 61}},
        {ui->aboutLabel3, {140, 190, 550, 110}},
        {ui->changeKeysButton, {310, 360, 180, 40}},
        {ui->usernameButton, {630, 5, 160, 20}},
        {this->openChatButton, {280, 0, 241, 22}},
        {this->chatArea, {285, 19, 230, 201}},
        {this->chatLine, {285, 200, 230, 19}}
    };

    // Calculate scaling factors
    float widthScale = this->width() / static_cast<float>(defaultSize.width());
    float heightScale = this->height() / static_cast<float>(defaultSize.height());

    // Apply scaling factors to each widget
    for (auto i = defaultProperties.begin(); i != defaultProperties.end(); ++i) {
        i.key()->setGeometry(i.value().x * widthScale,
                             i.value().y * heightScale,
                             i.value().width * widthScale,
                             i.value().height * heightScale);
        if (i.key() == ui->aboutLabel1 || i.key() == ui->changeKeysButton) {
            QFont font = i.key()->font();
            font.setPointSize(16.0 * widthScale);
            i.key()->setFont(font);
        } else if (i.key() == ui->aboutLabel2 || i.key() == ui->aboutLabel3 ||
                   i.key() == ui->aboutButton || i.key() == ui->createButton ||
                   i.key() == ui->joinButton || i.key() == ui->usernameButton) {
            QFont font = i.key()->font();
            font.setPointSize(9.0 * widthScale);
            i.key()->setFont(font);
        } else if (i.key() == ui->playButton || i.key() == ui->localButton || i.key() == this->startButton) {
            QFont font = i.key()->font();
            font.setPointSize(13.0 * widthScale);
            i.key()->setFont(font);
        } else if (i.key() == ui->howToButton) {
            QFont font = i.key()->font();
            font.setPointSize(7.0 * widthScale);
            i.key()->setFont(font);
        }
    }

    // Connection button will always be on the bottom right
    ui->connectionInfoLabel->setGeometry(this->width() - 240, this->height() - 26, 210, 20);
    ui->connectButton->setGeometry(this->width() - 20, this->height() - 20, 10, 10);

    // If there is overlay widget, selection view, resize it as well
    if (this->overlay != nullptr) this->overlay->setGeometry(0, 0, this->width(), this->height());
    if (this->selectionView != nullptr) this->selectionView->setGeometry(0, 0, this->width(), this->height());
    if (this->gameView != nullptr) this->gameView->setGeometry(0, 0, this->width(), this->height());

}

// If a player resize the window, it should keep the aspect ratio
void MainWindow::resizeEvent(QResizeEvent *event) {

    // This alone will open the window in this aspect ratio even if the screen size is different
    float aspectRatio = 16.0f / 9.0f;

    // What should be the new dimensions
    // If you only change the width it won't change the newWidth. Same applies for height and newHeight
    int newWidth = event->size().height() * aspectRatio;
    int newHeight = event->size().width() / aspectRatio;

    // Which side should be adjusted? You can reduce the size but can't increase it unless you do it diagonally
    if (newWidth > event->size().width()) {  // If you increase the height
        newWidth = event->size().width();    // The width should stay the same. newHeight will already be sized to the width
    } else {                                 // If you reduce the height
        newHeight = event->size().height();  // The width is already sized to the height. So keep this height
    }

    this->resize(newWidth, newHeight);
    this->resizeWidgets();                   // Resize widgets
    this->scale = this->height() / 1080.0;  // The scale will be used in a lot of parts of the game

}

// closeEvent is important for disconnecting from server
void MainWindow::closeEvent(QCloseEvent *event) {
    // Disconnect from the server if it's connected
    if (this->connectedServer == true) {
        QJsonObject obj;
        obj["type"] = 8;
        obj["id"] = this->id;
        obj["room"] = this->currentRoom;
        QByteArray data = QJsonDocument(obj).toJson();
        this->socket->writeDatagram(data, QHostAddress::LocalHost, 3000);
    }

    event->accept();
}

MainWindow::~MainWindow() {
    delete ui;
    delete overlay;
    delete exitRoomButton;
    delete startButton;
    delete openChatButton;
    delete chatArea;
    delete chatLine;
    delete selection;
    delete selectionView;
    delete gameTime;
    delete scene1;
    delete scene2;
    delete scene3;
    delete healthBar1;
    delete healthBar2;
    delete player1;
    delete player2;
    delete gameScene;
    delete gameView;
    delete gameModal;
    delete soundPlayer1;
    delete soundPlayer2;
    delete musicOutput;
    delete musicPlayer;
    delete socket;

    overlay = nullptr;
    exitRoomButton = nullptr;
    startButton = nullptr;
    openChatButton = nullptr;
    chatArea = nullptr;
    chatLine = nullptr;
    selection = nullptr;
    selectionView = nullptr;
    gameTime = nullptr;
    scene1 = nullptr;
    scene2 = nullptr;
    scene3 = nullptr;
    healthBar1 = nullptr;
    healthBar2 = nullptr;
    player1 = nullptr;
    player2 = nullptr;
    gameScene = nullptr;
    gameView = nullptr;
    gameModal = nullptr;
    soundPlayer1 = nullptr;
    soundPlayer2 = nullptr;
    musicOutput = nullptr;
    musicPlayer = nullptr;
    socket = nullptr;
}
