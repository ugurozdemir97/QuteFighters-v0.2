#include "mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
//#include "./ui_mainwindow.h"

// Show the winner when the game is finished
void MainWindow::gameFinished() {
    QString whoWin;

    // Calculate the health percentage, player with more health will win
    float player1HealthPercentage = this->player1->health * 100 / this->healthBar1->maxHealth;
    float player2HealthPercentage = this->player2->health * 100 / this->healthBar2->maxHealth;

    if (player1HealthPercentage > player2HealthPercentage) whoWin = this->selection->player1Name->toPlainText() + " Won!";
    else if (player1HealthPercentage < player2HealthPercentage) whoWin = this->selection->player2Name->toPlainText() + " Won!";
    else whoWin = "Tie!";

    this->gameTime->showWinner(whoWin);
    QTimer::singleShot(2500, this, &MainWindow::exitGame);
}

// Check hitbox collision
void MainWindow::checkHitboxCollision(Player &one, Player &two, HealthBar &healthBar) {

    // Check if the hitbox collides with the opponent's hurtbox
    if (one.hitbox->collidesWithItem(two.hurtbox)) {

        one.setZValue(2);  // Hitting character should be above the other one
        two.setZValue(1);

        two.velocityX = 0;

        // Which attack the player is doing?
        int damage = 0;
        switch (one.lastAttack) {
            case Player::NL:
                damage = one.hitboxPos["nL"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::NH:
                damage = one.hitboxPos["nH"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::SL:
                damage = one.hitboxPos["sL"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::SH:
                damage = one.hitboxPos["sH"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::DL:
                damage = one.hitboxPos["dL"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::DH:
                damage = one.hitboxPos["dH"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::AL:
                damage = one.hitboxPos["aL"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            case Player::AH:
                damage = one.hitboxPos["aH"][QString::number(one.currentFrame)]["damage"].toInt() * one.strength;
                break;
            default:
                break;
        }

        if (two.doing == Player::Block) damage /= 3;
        else if (two.doing == Player::CrouchBlock) damage /= 2;  // Crouch block is weaker than normal block because it already makes hurtbox smaller
        else {two.doing = Player::Hit; two.currentAnimation = 0;}
        two.health -= damage;
        healthBar.shortenHealth(two.health);  // Show the damage on the health bar
    }

    // If the opponent's health is less equal zero, the game ends
    if (two.health <= 0) {
        this->gameTime->stopTimers();
        this->gameFinished();
    }
}

// Move the players in XY directions with some limitations
void MainWindow::movePlayers(Player &one, Player &two) {

    // If going left
    if (one.velocityX < 0) {

        // If the distance is less than the maximum distance
        if (two.x() - one.x() < this->maxDistance) {

            // If the player is on left limit, the camera should move instead, moving the other player outwards.
            if ((one.facing == Player::Left && one.x() <= one.frontLimitLeft) ||
                (one.facing == Player::Right && one.x() <= one.backLimitLeft)) {

                // If we are not at the scene limit, move the scene and push the opponent
                if (this->sceneLimitLeft <= this->scene1->x()) {
                    this->scene1->moveScene(1, one.pushScene1, one.skewScene);
                    this->scene2->moveScene(1, one.pushScene2);
                    this->scene3->moveScene(1, one.pushScene3);
                    two.pushRight();

                    // Otherwise prevent moving
                } else {
                    if (one.doing == Player::Walk && one.facing == Player::Right) {
                        one.doing = Player::Idle;
                        one.velocityX = 0;
                    } else {
                        // If players' hurtboxes are not colliding, move the player normally
                        if (!one.hurtbox->collidesWithItem(two.hurtbox)) {
                            one.setX(one.x() - one.speed);
                            // Otherwise push the opponent as you move
                        } else {
                            if (one.x() <= two.x()) one.pushLeft();  // We are pushed to the left
                            else two.pushLeft();                     // We are pushing the opponent
                        }
                    }
                }

                one.setY(one.y() + one.velocityY);  // We will still move in Y direction

                // This is just the normal situation, we are not in the edge and moving
            } else {

                // If players' hurtboxes are not colliding, move the player normally
                if (!one.hurtbox->collidesWithItem(two.hurtbox)) {
                    one.setPos(one.x() - one.speed, one.y() + one.velocityY);

                    // Otherwise push the opponent as you move
                } else {
                    one.setY(one.y() + one.velocityY);
                    if (one.x() <= two.x()) one.pushLeft();  // We are pushed to the left
                    else two.pushLeft();                     // We are pushing the opponent
                }
            }

            // If the distance is equal or greater than the maximum distance, prevent moving
        } else {
            if (one.doing == Player::Walk)  one.doing = Player::Idle;
            one.velocityX = 0;
            one.setY(one.y() + one.velocityY);
        }

        // If going right
    } else if (one.velocityX > 0) {
        // If the distance is less than the maximum distance
        if (one.x() - two.x() < this->maxDistance) {

            // If the player is on right limit, the camera should move instead, moving the other player outwards.
            if ((one.facing == Player::Left && one.x() >= one.backLimitRight) ||
                (one.facing == Player::Right && one.x() >= one.frontLimitRight)) {

                // If we are not at the scene limit, move the scene and push the opponent
                if (this->sceneLimitRight >= this->scene1->x()) {
                    this->scene1->moveScene(0, one.pushScene1, one.skewScene);
                    this->scene2->moveScene(0, one.pushScene2);
                    this->scene3->moveScene(0, one.pushScene3);
                    two.pushLeft();

                    // Otherwise prevent moving
                } else {
                    if (one.doing == Player::Walk && one.facing == Player::Left) {
                        one.doing = Player::Idle;
                        one.velocityX = 0;
                    } else {
                        // If players' hurtboxes are not colliding, move the player normally
                        if (!one.hurtbox->collidesWithItem(two.hurtbox)) {
                            one.setX(one.x() + one.speed);
                            // Otherwise push the opponent as you move
                        } else {
                            if (one.x() <= two.x()) two.pushRight();  // We are pushing the opponent
                            else one.pushRight();                     // We are pushed to the right
                        }
                    }
                }

                one.setY(one.y() + one.velocityY);  // We will still move in Y direction

                // This is just the normal situation, we are not in the edge and moving
            } else {

                // If players' hurtboxes are not colliding, move the player normally
                if (!one.hurtbox->collidesWithItem(two.hurtbox)) {
                    one.setPos(one.x() + one.speed, one.y() + one.velocityY);

                    // Otherwise push the opponent as you move
                } else {
                    one.setY(one.y() + one.velocityY);
                    if (one.x() <= two.x()) two.pushRight();  // We are pushing the opponent
                    else one.pushRight();                     // We are pushed to the right
                }
            }

            // If the distance is equal or greater than the maximum distance, prevent moving
        } else {
            if (one.doing == Player::Walk) one.doing = Player::Idle;
            one.velocityX = 0;
            one.setY(one.y() + one.velocityY);
        }

        // If standing still
    } else {

        // If hurtboxes are not colliding, move in Y direction only
        if (!one.hurtbox->collidesWithItem(two.hurtbox)) {
            one.setY(one.y() + one.velocityY);
        } else {

            // Otherwise both players should be pushed further away from each other
            one.setY(one.y() + one.velocityY);
            if (one.x() <= two.x()) {
                one.pushLeft();
                two.pushRight();
            } else {
                one.pushRight();
                two.pushLeft();
            }
        }
    }

    if (one.x() < one.backLimitLeft && this->sceneLimitLeft > this->scene1->x()) one.setX(one.backLimitLeft);
    else if (one.x() > one.backLimitRight && this->sceneLimitRight < this->scene1->x()) one.setX(one.backLimitRight);

    // Change velocity in Y direction if the player is not on the ground or on the ground but have a negative velocity
    // Meaning, if the player is jumped
    if (one.y() < one.onGround || (one.y() == one.onGround && one.velocityY < 0)) {

        // If not already falling or doing an aerial attack, change action to Fall
        if (one.doing != Player::Fall && one.velocityY >= 0 && one.doing != Player::AL && one.doing != Player::AH) {
            one.doing = Player::Fall;
            one.currentAnimation = 0;
        }

        // Increase velocity so that the player will slow down as it gets higher and eventually will start to fall
        one.velocityY += one.fallSpeed;
    }

    // If the player is fell, reset the position in Y just in case, make the player eighter walk or stay still.
    if (one.y() >= one.onGround && (one.velocityY > 0 || one.doing == Player::Fall)) {
        one.velocityY = 0;
        one.setY(one.onGround);
        if (one.doing == Player::Fall) one.afterAction();
    }

    // Change which direction the player is facing if necessary
    if (one.x() < two.x() && one.facing == Player::Left) one.facing = Player::Right;
    else if (one.x() > two.x() && one.facing == Player::Right) one.facing = Player::Left;

}

void MainWindow::gameLogic() {

    // Play the animations, sprite sheets. Move hurtboxes and activate the hitboxes if necessary
    this->player1->animate();
    this->player2->animate();

    // Move the players in XY directions, prevent their hurtboxes from intersecting each other, move the screen if necessary
    this->movePlayers(*this->player1, *this->player2);
    this->movePlayers(*this->player2, *this->player1);

    // Check hitbox collision if a hitbox is active
    if (this->player1->hitboxActive)
        this->checkHitboxCollision(*this->player1, *this->player2, *this->healthBar2);
    if (this->player2->hitboxActive)
        this->checkHitboxCollision(*this->player2, *this->player1, *this->healthBar1);

    this->player1->update();
    this->player2->update();

    //std::vector<QByteArray> gameStateHistory;       // Current position, action and health of the players
    //void rollBack(Player &one, Player &two);        // Go back to the last confirmed state from the and process events

    // Move the shadows under the players
    //this->moveShadows();

    //this->gameScene->update();
    //this->gameView->viewport()->update();

    //qDebug() << this->FPStest;
    //this->FPStest++;
}

// This can be called if you close the game modal, continue game time if it's stopped
void MainWindow::gameModalClosed() {
    this->gameModal = nullptr;
    this->gameTime->startTimers();
}

/*

// You just setX position of the shadows same as players x position
void MainWindow::moveShadows() {
    this->player1Shadow->currentFrame = this->player1->currentFrame;
    this->player2Shadow->currentFrame = this->player2->currentFrame;
    this->player1Shadow->setX(this->player1->x());
    this->player2Shadow->setX(this->player2->x());
}

*/
