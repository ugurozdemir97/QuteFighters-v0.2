# QuteFighters
QuteFighters is a 2D fighting game that allows two players to play locally or online. I developed this game to learn about the Qt framework and C++. I created all the animations, drawings, and sound effects myself. The name "Qute Fighters" comes from the framework I used "Qt" and that I find the characters to be "cute". The game is not complete yet. The online gameplay has synchronization issues that I need to address by adding a client prediction and roll back mechanism but local gameplay has no issues.

## Setup:
Just download the files and run QuteFighters.exe
24 mb is images, music and sound effects and the source codes. The rest of the data comes from the necessary dll's

Download link: <a href="https://drive.google.com/file/d/1b0k6kSXr2B9gMp1BpQbtFE4w8c1IBZUX/view?usp=sharing">Qute Fighters</a>

If you want to test its online feature (read more about it in the "Online Gameplay" section, you don't need to download this for the game to run locally). 

Server's source code: <a href="https://github.com/ugurozdemir97/Qute-Fighters-Server">Qute Fighters Server</a> 

## Keys:
You can change key bindings of both players from How To Play section. In default:

Player 1 uses:
<ul>
  <li>"WASD" keys to move.</li>
  <li>"G" for light attack and character selection.</li>
  <li>"J" for heavy attack and deselection.</li>
  <li>"Y" for blocking attacks.</li>
</ul>  
  
Player 2 uses:
<ul>
  <li>"Arrow keys" to move.</li>
  <li>"İ" for light attack and character selection (If you are not using TurkishQ keyboard you may need to change this).</li>
  <li>"L" for heavy attack and deselection.</li>
  <li>"P" for blocking attacks.</li>
</ul>  

![Key](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/10f65522-6019-45ad-b23e-5fa5ba4acc44)
  
## Characters:
Currently, there are two characters with identical movesets but different stats. The red character is faster but has less health.

## Map:
The map consists of three layers, each moving at different speeds depends on the characters' speed to create a 3D parallax effect. The bottom layer also skews and transforms to create perspective as the players move. Players can move until they reach the walls. The players can't move more further away from each other than the screen width even if they are not touching the walls so it will guarantee the characters will always be in the visible portion of the screen.

![Map](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/56c1f941-ad58-4e0e-a073-53367405d526)

## Moves:
In the future I may add special attacks for each character but the moveset logic will always be the same. There are six grounded and two aerial attacks. To perform these attacks, players must combine movement keys with attack keys, similar to the game "Brawlhalla".

Light attack (neutral light / nL), light attack while moving towards the opponent (side light / sL), light attack while crouching (down light / dL).
The same pattern applies to heavy attacks (nH, sH, dH).
Players can perform aerial light (aL) and aerial heavy (aH) attacks in the air.
There is also blocking, which can be used while standing or crouching. Blocking does not prevent damage but reduces it.

![Moves](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/23b76571-8500-4c7a-b13e-e60708da10b3)

## Online Gameplay
The game has an UDP server written in Node.js. If you run the server locally it's technically possible to play the game online but it has synchronization problems sometimes. I will address this in the future. After you run the server and open the game, you should see ‘Successfully connected to the server’ at the bottom right. If you click that button it will disconnect you, by clicking again you can connect back to the server. When you are connected, you can create rooms. Other players can join that room using the room number. 

![Join](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/867396b9-4a09-49e8-8458-25f15f8f842a)

In that room, you can chat with everyone who is in the room using the chat bar on the top. 

![Chat](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/56e6073c-6231-4b75-a1e9-6ba033e0afcb)

There will be only two players but there is no limit for spectators. Player 1 is the room leader and can start the game when both players select their character. If you click your username in the main menu on the top right, you can change your username, other players will see your username when you are chatting or playing with them.

Here is two players playing online.

![OnlinePlay](https://github.com/ugurozdemir97/QuteFighters-v0.2/assets/64408736/b70d3e77-6f56-4411-ac9e-ef843da1e60f)

### Game Ends If:
The game ends when a player’s health reaches zero or when time runs out. If time expires, the player with more health percentage wins. You can see both the timer and health bars on the screen when the game begins.
