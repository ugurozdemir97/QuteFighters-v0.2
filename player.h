#ifndef PLAYER_H
#define PLAYER_H
#include <QGraphicsItem>
#include <QJsonValue>
#include <QObject>

// This class handles players actions, hitboxes and hurtboxes and keep track of their health
class Player : public QObject, public QGraphicsItem {
    Q_OBJECT  // You need this to have as signal in the class
    public:
        Player(const QPixmap &pixmap, QJsonObject character, int pushSpeed, bool direction,
               double scale, float left, float right, float height, QGraphicsItem *parent = nullptr);
        ~Player();

        // *** WHAT CAN THE CHARACTER DO *** //

        enum Direction {Left, Right};
        enum Action {Idle, Walk, Crouch, Stand, Jump, PreJump, Fall, Block, CrouchBlock, Hit, NL, NH, SL, SH, DL, DH, AL, AH};
        Direction facing = Left;   // Where the player is looking at
        Action doing = Idle;       // What the player is doing
        Action lastAttack;         // Last performed attack, this will be used to prevent players from spamming one attack again and again

        // *** FRAMES AND POSITIONING *** //

        int currentFrame = 0;      // Current frame
        int currentAnimation = 0;  // If you are running or doing x, which frame of running or x animation are you in

        std::map<std::string, std::vector<int>> animations;  // A map that holds arrays filled with animation frames info
        QJsonValue hurtboxPos;  // A JSON object that holds the hurtbox positions. For example if you are in 23rd frame where the hurtbox should be
        QJsonValue hitboxPos;   // Same for hitboxes when you attack, holds the info about where the hitbox is

        QGraphicsEllipseItem *hitbox = nullptr; // Hitbox
        QGraphicsRectItem *hurtbox = nullptr;   // Hurtbox

        // *** CHARACTER'S INFO *** //

        int health = 0;         // Every characters can have different health, strength, speed
        float strength = 0;
        float speed = 0;        // Your speed, used to move yourself
        float jumpSpeed = 0;
        float fallSpeed = 0;

        float pushSpeed = 0;    // Your opponent's speed, used to push you outer by your opponent
        float pushScene1 = 0;   // How much the scenes should move, scenes' movement speed depends on your speed
        float pushScene2 = 0;
        float pushScene3 = 0;
        float skewScene = 0;    // How much the floor should be skewed to create some perspective and 3D parallax effect

        float velocityX = 0;    // Your velocity in X and Y directions
        float velocityY = 0;

        float onGround = true;      // The Y value of the character when the character is on the ground

        bool moveKeyLeft = false;   // If move keys are pressed or not, used to decide what action to do after some moves
        bool moveKeyRight = false;
        bool moveKeyDown = false;

        bool hitboxActive = false;  // If the hitboxes are active or not

        // *** MOVEMENT METHODS *** //

        // These methods just change the current animation, this->doing, and velocity.

        // If the other player moves outside, MainWindow will call this function to push your character outer
        // Also if the other player pushes you, you will be pushed
        void pushLeft();
        void pushRight();

        // All the actions that a player can do
        void moveLeft();     // Move left or right
        void moveRight();
        void jump();         // Jump
        void crouch();       // Crouch and stand up. Stand up will be called if you release a key
        void stand();
        void idle();         // If you do nothing
        void block();        // Block standing
        void crouchBlock();  // Crouching block
        void nL();           // Neutral Light Attack
        void sL();           // Side Light Attack
        void dL();           // Down Light Attack
        void aL();           // Aerial Light Attack
        void nH();           // Neutral Heavy Attack
        void sH();           // Side Heavy Attack
        void dH();           // Down Heavy Attack
        void aH();           // Aerial Heavy Attack
        void afterAction();  // Recover after an action, eighter turn back to walking or standing still

        // *** ANIMATE *** //

        void activateHitbox(QString attack);  // Activate the hitbox if the character is doing an attack
        void moveHurtbox();                   // Your hurtboxes will move as your current frame changes.
        void animate();                       // Calculate the current frame, animation

        float frontLimitLeft = 0.0f;  // This is how further the players can move in the map
        float backLimitLeft = 0.0f;   // This depends on other player's character's frame width, so I need a method to set it in MainWindow
        float frontLimitRight = 0.0f;
        float backLimitRight = 0.0f;

        void setLimits(float width, float left, float right, float scale);  // Set the frontLimit and backLimit

        int frameWidth = 0;   // These are the size of an individual frame in the sprite sheet
        int frameHeight = 0;

    private:

        // Virtual methods of QGraphicsItem must be included.
        // The boundingRect method is changed to crop the rest of the sprite sheet.
        // The paint method is changed to draw only one frame of a sprite sheet. It also does calculations if the character turns right
        virtual QRectF boundingRect() const override;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        QPixmap pixmap;
        QRect frames[72];             // There are 72 (?) frames, all frames are QRect objects

        QDateTime lastAttackTime;  // To check if the cooldown condition of the attack is met or not
        QJsonValue sounds;         // Sound effects of the attacks

    signals:
        void playSound(QString soundPath);  // Emit this signal to the MainWindow to play the sound of a move
};

#endif // PLAYER_H
