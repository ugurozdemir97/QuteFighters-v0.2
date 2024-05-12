#include "player.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QApplication>

// This method is slighty changed to crop the rest of the sprite sheet
QRectF Player::boundingRect() const {return QRectF(0, 0, this->frameWidth, this->frameHeight);}

// This method is slightly changed to draw only one frame from the sprite sheet
void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (this->facing == Left) {
        painter->drawPixmap(QPointF(0, 0), pixmap, frames[this->currentFrame]);
    } else {
        // I need to reverse the frame. For example frame 2 should be 7. Frame 9 should be 17, frame 21 should be 23... etc.
        // In order to do that we need to determine which row we are in.
        // ((int (frame / 9)) + 1) * 9 -> This will give you 9, 18, 27... The row information
        // - frame - 1 -> this will give you the index. For example for frame 2 you will get 6. For frame 9 you will get -1
        // finally + int (frame / 9) * 9 -> This will give you the final result.
        // There might be better calculations. This is what I could came up with.
        int newFrame = (((this->currentFrame / 9) + 1) * 9) - (this->currentFrame + 1) + ((this->currentFrame / 9) * 9);
        painter->drawPixmap(QPointF(0, 0), pixmap.transformed(QTransform().scale(-1, 1)), frames[newFrame]);

    }
}

void Player::setLimits(float width, float left, float right, float scale) {
    this->frontLimitLeft = left + (width * scale);
    this->frontLimitRight = right - ((width + this->frameWidth) * scale);

    this->backLimitLeft = left;
    this->backLimitRight = right - (this->frameWidth * scale);
}

// ************** ANIMATE *************** //

// Activate the hitboxes and move them
void Player::activateHitbox(QString attack) {
    // If there is active hitbox
    if (this->hitboxPos[attack][QString::number(this->currentFrame)] != QJsonValue::Undefined) {
        int x = this->hitboxPos[attack][QString::number(this->currentFrame)]["x"].toInt();
        int y = this->hitboxPos[attack][QString::number(this->currentFrame)]["y"].toInt();
        if (this->facing == Right) x = this->frameWidth - (x + 80);
        this->hitbox->setPos(x, y);
        this->hitboxActive = true;
    }
}

// Move hurtbox as the current frame changes
void Player::moveHurtbox() {
    int x = this->hurtboxPos[QString::number(this->currentFrame)]["x"].toInt();
    int y = this->hurtboxPos[QString::number(this->currentFrame)]["y"].toInt();
    if (this->facing == Right) x = this->frameWidth - (x + this->hurtboxPos["width"].toInt());
    this->hurtbox->setPos(x, y);
}

// Animate the players as the current frame changes, this method just changes the currentFrame value
void Player::animate() {

    if (this->hitboxActive) this->hitboxActive = false;  // If the hitboxes are active, deactivate them

    switch (this->doing) {
        case Idle:
            if (this->animations["idle"].size() <= this->currentAnimation) this->currentAnimation = 0;
            this->currentFrame = this->animations["idle"][this->currentAnimation];
            break;
        case Walk:
            if (this->animations["walk"].size() <= this->currentAnimation) {this->currentAnimation = 0;}
            if ((this->facing == Left && this->velocityX < 0) || (this->facing == Right && this->velocityX > 0)) {
                this->currentFrame = this->animations["walk"][this->currentAnimation];
            } else {
                this->currentFrame = this->animations["walkReverse"][this->currentAnimation];
            }
            break;
        case Crouch:
            if (this->animations["crouch"].size() <= this->currentAnimation) this->currentAnimation = this->animations["crouch"].size() - 1;
            this->currentFrame = this->animations["crouch"][this->currentAnimation];
            break;
        case Stand:
            if (this->animations["stand"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["stand"][this->currentAnimation];
            }
            break;
        case Jump:
            if (this->animations["jump"].size() <= this->currentAnimation) this->currentAnimation = this->animations["jump"].size() - 1;
            this->currentFrame = this->animations["jump"][this->currentAnimation];
            break;
        case PreJump:
            if (this->animations["preJump"].size() <= this->currentAnimation) {
                this->currentAnimation = - 1;
                this->velocityY = this->jumpSpeed;  // A number, should be the same for every character
                this->doing = Jump;
                //emit playSound(this->sound["jump"].toString());
            } else {
                this->currentFrame = this->animations["preJump"][this->currentAnimation];
            }
            break;
        case Fall:
            if (this->animations["fall"].size() <= this->currentAnimation) this->currentAnimation = this->animations["fall"].size() - 1;
            this->currentFrame = this->animations["fall"][this->currentAnimation];
            break;
        case Block:
            if (this->animations["standBlock"].size() <= this->currentAnimation) this->currentAnimation = 0;
            this->currentFrame = this->animations["standBlock"][this->currentAnimation];
            break;
        case CrouchBlock:
            if (this->animations["crouchBlock"].size() <= this->currentAnimation) this->currentAnimation = 0;
            this->currentFrame = this->animations["crouchBlock"][this->currentAnimation];
            break;
        case Hit:
            if (this->animations["hit"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->doing = Idle;
            } else {
                if (this->currentAnimation == 0) emit playSound(this->sounds["hit"].toString());
                this->currentFrame = this->animations["hit"][this->currentAnimation];
            }
            break;
        case Player::NL:
            if (this->animations["nL"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["nL"][this->currentAnimation];
                this->activateHitbox("nL");
            }
            break;
        case Player::SL:
            if (this->animations["sL"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["sL"][this->currentAnimation];
                this->activateHitbox("sL");
            }
            break;
        case Player::DL:
            if (this->animations["dL"].size() <= this->currentAnimation) {
                this->currentAnimation = this->animations["crouch"].size() - 2;
                if (this->moveKeyDown) this->doing = Player::Crouch;
                else this->doing = Player::Stand;
            } else {
                this->currentFrame = this->animations["dL"][this->currentAnimation];
                this->activateHitbox("dL");
            }
            break;
        case Player::NH:
            if (this->animations["nH"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["nH"][this->currentAnimation];
                this->activateHitbox("nH");
            }
            break;
        case Player::SH:
            if (this->animations["sH"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["sH"][this->currentAnimation];
                this->activateHitbox("sH");
            }
            break;
        case Player::DH:
            if (this->animations["dH"].size() <= this->currentAnimation) {
                this->currentAnimation = -1;
                this->afterAction();
            } else {
                this->currentFrame = this->animations["dH"][this->currentAnimation];
                this->activateHitbox("dH");
            }
            break;
        case Player::AL:
            if (this->animations["aL"].size() <= this->currentAnimation) {
                this->currentAnimation = - 1;
                if (this->y() >= this->onGround) {
                    this->afterAction();
                } else {
                    this->doing = Player::Fall;
                }
            } else {
                this->currentFrame = this->animations["aL"][this->currentAnimation];
                this->activateHitbox("aL");
            }
            break;
        case Player::AH:
            if (this->animations["aH"].size() <= this->currentAnimation) {
                this->currentAnimation = - 1;
                if (this->y() >= this->onGround) {
                    this->afterAction();
                } else {
                    this->doing = Player::Fall;
                }
            } else {
                this->currentFrame = this->animations["aH"][this->currentAnimation];
                this->activateHitbox("aH");
            }
            break;
    }

    this->currentAnimation++;
    this->moveHurtbox();

}

// ************** MOVEMENT ************** //

// Move the character if other player is moving outside of the visible portion of QGraphicsScene or literally pushing our player
// This should move the character with the same speed as ground.
void Player::pushLeft() {this->setPos(this->x() - this->pushSpeed, this->y());}
void Player::pushRight() {this->setPos(this->x() + this->pushSpeed, this->y());}

// You can move left if you are standing still or walking to the right
void Player::moveLeft() {
    if ((this->doing == Idle || this->doing == Walk) && this->velocityX >= 0) {
        this->velocityX = (-1 * this->speed);
        this->doing = Walk;
        this->currentAnimation = 0;
    }
}

// Right walk
void Player::moveRight() {
    if ((this->doing == Idle || this->doing == Walk) && this->velocityX <= 0) {
        this->velocityX = this->speed;
        this->doing = Walk;
        this->currentAnimation = 0;
    }
}

// Jump, this method makes the character prepare for jump, in these frames tha player's Y velocity will remain 0
// After the preJump animations finished, the player will have a negative Y velocity and jump.
void Player::jump() {
    if ((this->doing == Idle || this->doing == Walk) && this->velocityY == 0) {
        this->doing = PreJump;  // After the preparation for jump animations are finished, the player will jump
        this->currentAnimation = 0;
    }
}

// Crouch
void Player::crouch() {
    if (this->doing == Idle || this->doing == Walk || this->doing == CrouchBlock) {
        this->velocityX = 0;
        this->velocityY = 0;
        this->doing = Crouch;
        this->currentAnimation = 0;
    }
}

// Standing up from crouching
void Player::stand() {
    if (this->doing == Crouch) {
        this->velocityX = 0;
        this->velocityY = 0;
        this->doing = Stand;
        this->currentAnimation = 0;
    }
}

// Standing still
void Player::idle() {
    if (this->doing == Idle || this->doing == Walk || this->doing == Block) {
        this->velocityX = 0;
        this->velocityY = 0;
        this->doing = Idle;
        this->currentAnimation = 0;
    }
}

// Block
void Player::block() {
    if (this->doing == Idle || this->doing == Walk) {
        this->velocityX = 0;
        this->velocityY = 0;
        this->doing = Block;
        this->currentAnimation = 0;
    }
}

// Crouch block
void Player::crouchBlock() {
    if (this->doing == Crouch) {
        this->velocityX = 0;
        this->velocityY = 0;
        this->doing = CrouchBlock;
        this->currentAnimation = 0;
    }
}

// Neutral Light Attack Or Aerial Light Attack
void Player::nL() {
    if (this->doing == Idle) {
        if (this->lastAttack != NL || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["nL"]["cooldown"].toInt()) {
            this->doing = NL;
            this->lastAttack = NL;
            this->lastAttackTime = QDateTime::currentDateTime();
            this->currentAnimation = 0;
            emit playSound(this->sounds["nL"].toString());
        }
    } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (18))) {
        this->aL();
    }
}

// Side Light Attack (Or Neutral Air Light)
void Player::sL() {
    if ((this->facing == Left && this->velocityX <= 0) || (this->facing == Right && this->velocityX >= 0)) {
        if (this->doing == Walk) {
            if (this->lastAttack != SL || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["sL"]["cooldown"].toInt()) {
                this->doing = SL;
                this->lastAttack = SL;
                this->lastAttackTime = QDateTime::currentDateTime();
                this->velocityX = 0;
                this->currentAnimation = 0;
                emit playSound(this->sounds["sL"].toString());
            }
        } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (18))) {
            this->aL();
        }
    }
}

// Down Light Attack (Or Neutral Air Light)
void Player::dL() {
    if (this->doing == Crouch) {
        if (this->lastAttack != DL || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["dL"]["cooldown"].toInt()) {
            this->doing = DL;
            this->lastAttack = DL;
            this->lastAttackTime = QDateTime::currentDateTime();
            this->currentAnimation = 0;
            emit playSound(this->sounds["dL"].toString());
        }
    } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (18))) {
        this->aL();
    }
}

void Player::aL() {
    this->doing = AL;
    this->lastAttack = AL;
    this->lastAttackTime = QDateTime::currentDateTime();
    this->currentAnimation = 0;
    emit playSound(this->sounds["aL"].toString());
}

// Neutral Heavy Attack (Or Neutral Air Heavy)
void Player::nH() {
    if (this->doing == Idle) {
        if (this->lastAttack != NH || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["nH"]["cooldown"].toInt()) {
            this->doing = NH;
            this->lastAttack = NH;
            this->lastAttackTime = QDateTime::currentDateTime();
            this->currentAnimation = 0;
            emit playSound(this->sounds["nH"].toString());
        }
    } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (54))) {
        this->aH();
    }
}

// Side Heavy Atttack (Or Neutral Air Heavy)
void Player::sH() {
    if ((this->facing == Left && this->velocityX <= 0) || (this->facing == Right && this->velocityX >= 0)) {
        if (this->doing == Walk) {
            if (this->lastAttack != SH || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["sH"]["cooldown"].toInt()) {
                this->doing = SH;
                this->lastAttack = SH;
                this->lastAttackTime = QDateTime::currentDateTime();
                this->velocityX = 0;
                this->currentAnimation = 0;
                emit playSound(this->sounds["sH"].toString());
            }
        } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (54))) {
            this->aH();
        }
    }
}

// Down Heavy Attack (Or Neutral Air Heavy)
void Player::dH() {
    if (this->doing == Crouch) {
        if (this->lastAttack != DH || lastAttackTime.msecsTo(QDateTime::currentDateTime()) >= this->hitboxPos["dH"]["cooldown"].toInt()) {
            this->doing = DH;
            this->lastAttack = DH;
            this->lastAttackTime = QDateTime::currentDateTime();
            this->currentAnimation = 0;
            emit playSound(this->sounds["dH"].toString());
        }
    } else if (this->doing == Jump || (this->doing == Fall && this->y() < this->onGround - (54))) {
        this->aH();
    }
}

void Player::aH() {
    this->doing = AH;
    this->lastAttack = AH;
    this->lastAttackTime = QDateTime::currentDateTime();
    this->currentAnimation = 0;
    emit playSound(this->sounds["aH"].toString());
}

// Recover after doing an action
void Player::afterAction() {
    this->currentAnimation = -1;

    // If left or right keys are pressed, continue moving left or right
    if (this->moveKeyLeft || this->moveKeyRight) {
        this->doing = Player::Walk;
        if (this->moveKeyLeft) this->moveLeft();
        else this->moveRight();

    // Otherwise check if crouch key is pressed, crouch if it's pressed, otherwise just stay still
    } else {
        if (this->moveKeyDown) {
            this->doing = Player::Crouch;
        } else {
            this->doing = Player::Idle;
            this->velocityX = 0;
        }
    }
}


Player::Player(const QPixmap &pixmap, QJsonObject character, int pushSpeed, bool direction,
               double scale, float left, float right, float height, QGraphicsItem *parent) :
    QGraphicsItem(parent), pixmap(pixmap) {

    this->setScale(scale);
    this->setEnabled(false);

    // Character's frames
    this->frameWidth = character["frameWidth"].toInt();
    this->frameHeight = character["frameHeight"].toInt();

    // Define animation frames:
    for (int i = 0; i < 8; i++) {
        for (int k = 0; k < 9; k++) {
            this->frames[i * 9 + k] = QRect(k * this->frameWidth, i * this->frameHeight, this->frameWidth, this->frameHeight);
        }
    }

    // Place the character
    this->onGround = height - (this->frameHeight * scale);

    if (direction) {
        this->facing = Right;
        this->setPos(left, onGround);
    } else {
        this->setPos(right - (this->frameWidth * scale), onGround);
    }

    // *** Character's info *** //
    this->health = character["health"].toInt();
    this->strength = character["strength"].toDouble() / 500;
    this->jumpSpeed = -110 * scale;
    this->fallSpeed = 15 * scale;
    this->speed =  (character["speed"].toInt() / 10) * scale;  // Our speed
    this->pushSpeed = (pushSpeed / 10) * scale;  // Other player's speed. This is how fast should we pushed

    // Through trial and error, and some math, I found out that if the player speed is 17,
    // scene1 should be 27, scene2 should be 8 and scene3 should be 12. Skew angle should be 3.

    // So I will just find out their speed according to the selected character speed using a ratio. (X * (27 | 8 | 12)) / 17

    // But my scale is already 0.8, so if it is (27 | 8 | 12) / 17 in scale 0.8, it would be (33.75 | 10 | 15) / 21.25 in scale 1.0
    // I will calculate the speeds according to scale 1.

    this->pushScene1 = (character["speed"].toInt() / 10) * (33.75 / 21.25) * scale;
    this->pushScene2 = (character["speed"].toInt() / 10) * (10 / 21.25) * scale;
    this->pushScene3 = (character["speed"].toInt() / 10) * (15 / 21.25) * scale;
    this->skewScene = (character["speed"].toInt() / 10) * (3 / 21.25); // Skew is about angles and no matter what the scale is, the angle will stay the same.

    // *** Character's animation *** //

    // Information about animation frames for example walking is frame x to frame y
    QJsonObject animationObject = character["frames"].toObject();  // Create an object

    for (const QString &key : animationObject.keys()) {
        QJsonArray frameArray = animationObject[key].toArray();  // For each key ("run", "idle"... etc) create an array
        std::vector<int> frameVector;                            // Create an empty int vector consist of frame numbers
        for (const QJsonValue &value : frameArray) {             // Add all the elements from the array to the vector
            frameVector.push_back(value.toInt());
        }
        this->animations[key.toStdString()] = frameVector;       // Now add the key value pair to our animations field.

        // This animations field now consist of a data like this:
        // "run": [0, 1, 2], "idle": [3, 4, 3]....
    }

    this->hitboxPos = character["hitbox"].toObject();   // Hitbox positions
    this->hurtboxPos = character["hurtbox"].toObject(); // Hurtbox positions

    this->hitbox = new QGraphicsEllipseItem(this);
    this->hitbox->setRect(0, 0, 80, 80);
    this->hitbox->setBrush(QBrush(QColor(255, 100, 0, 60)));
    this->hitbox->setOpacity(0);

    this->hurtbox = new QGraphicsRectItem(this);
    this->hurtbox->setRect(0, 0, this->hurtboxPos["width"].toInt(), this->hurtboxPos["height"].toInt());
    this->hurtbox->setBrush(QBrush(QColor(0, 255, 0, 60)));
    this->hurtbox->setOpacity(0);

    this->sounds = character["sound"].toObject();

}

Player::~Player() {

    delete hitbox;
    delete hurtbox;

    hitbox = nullptr;
    hurtbox = nullptr;

}
