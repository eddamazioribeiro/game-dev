// #include "stdafx.h"
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// make code easier to type with "using namespace"
using namespace sf;

// function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
// Where is the player/branch? Left or Right?
enum class side{LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];



int main(){
    // create a video mode object
    VideoMode vm(960, 540);
    // create and open a window for the game
    RenderWindow window(vm, "Timber!", Style::Fullscreen);
    View view(sf::FloatRect(0, 0, 1920, 1080));
	window.setView(view);

    /* Textures */
    // create a texture to hold a graphic on the GPU
    Texture textureBackground;
    // load a graphic into the texture
    textureBackground.loadFromFile("./assets/graphics/background.png");
    // create a sprite
    Sprite spriteBackground;
    // attach the texture to the sprite
    spriteBackground.setTexture(textureBackground);
    // set the spriteBackground to cover the screen
    spriteBackground.setPosition(0, 0);
    // make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("./assets/graphics/tree.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(810, 0);
    // prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("./assets/graphics/bee.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 800);
    // is the bee currently moving?
    bool beeActive = false;
    // how fast can the bee fly
    float beeSpeed = 0.0f;
    // make 3 clouds sprites from 1 texture
    Texture textureCloud;
    // load 1 new texture
    textureCloud.loadFromFile("./assets/graphics/cloud.png");
    // 3 new sprites with the same texture
    Sprite spriteCloud1;
    Sprite spriteCloud2;
    Sprite spriteCloud3;
    spriteCloud1.setTexture(textureCloud);
    spriteCloud2.setTexture(textureCloud);
    spriteCloud3.setTexture(textureCloud);
    // position the clouds off screen
    spriteCloud1.setPosition(0, 0);
    spriteCloud2.setPosition(0, 250);
    spriteCloud3.setPosition(0, 500);
    // are the clouds currently on screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;
    // how fast is the cloud?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    // variavles to control time
    Clock clock;

    // time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // track whether the game is running
    bool paused = true;

    // draw some text
    int score = 0;

    sf::Text messageText;
    sf::Text scoreText;
    // choose the font
    Font font;
    font.loadFromFile("./assets/fonts/KOMIKAP_.ttf");
    // set the font
    messageText.setFont(font);
    scoreText.setFont(font);
    // assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setCharacterSize(100);
    // choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);
    // position the text
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);
    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

    scoreText.setPosition(20, 20);

    // prepare 6 branches
    Texture textureBranch;
    textureBranch.loadFromFile("./assets/graphics/branch.png");
    // set te texture for each branch sprite (using arrays)
    for(int i = 0; i < NUM_BRANCHES; i++){
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // set the sprites origins to dead center
        // we can then spin it round without changing its position
        branches[i].setOrigin(200, 0);

    }

    // prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("./assets/graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(580, 720);
    // the player starts on the left
    side playerSide = side::LEFT;

    // prepare the gravestone
    Texture textureRIP;
    textureRIP.loadFromFile("./assets/graphics/rip.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(600, 860);

    // prepare the axe
    Texture textureAxe;
    textureAxe.loadFromFile("./assets/graphics/axe.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(700, 830);

    // line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // prepare the flying log
    Texture textureLog;
    textureLog.loadFromFile("./assets/graphics/log.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(810, 720);
    // some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // control the player input
    bool acceptInput = false;

    // prepare the sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("./assets/sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("./assets/sound/death.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("./assets/sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    /* Main game loop */
    while(window.isOpen()){
        Event event;

        while(window.pollEvent(event)){
            if(event.type == Event::KeyReleased && !paused){
                // listen for key presses again
                acceptInput = true;

                // hide the axe
                spriteAxe.setPosition(2000,
                                      spriteAxe.getPosition().y);

            }

        }
        /* Handle the players input */
        if(Keyboard::isKeyPressed(Keyboard::Escape)){
            window.close();

        }

        // start the game
        if(Keyboard::isKeyPressed(Keyboard::Return)){
            paused = false;

            // reset the time and score
            score = 0;
            timeRemaining = 6;

            // make all the branches disappear
            for(int i = 0; i < NUM_BRANCHES; i++){
                branchPositions[i] = side::NONE;

            }
            // make sure the gravestone is hidden
            spriteRIP.setPosition(675, 2000);

            // move the player into position
            spritePlayer.setPosition(580, 720);

            acceptInput = true;

        }


        // wrap the player controls to make sure we're accepting input
        if(acceptInput){
            if(Keyboard::isKeyPressed(Keyboard::Right)){
                // make sure the player is on the right
                playerSide = side::RIGHT;
                score++;

                // add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_RIGHT,
                                      spriteAxe.getPosition().y);

                spritePlayer.setPosition(1200, 720);

                updateBranches(score);

                // set the log flying to the left
                spriteLog.setPosition(810, 720);
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;

                // play a chop sound
                chop.play();

            }

            if(Keyboard::isKeyPressed(Keyboard::Left)){
                // make sure the player is on the left
                playerSide = side::LEFT;
                score++;

                // add to the amount of time remaining
                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition(AXE_POSITION_LEFT,
                                      spriteAxe.getPosition().y);

                spritePlayer.setPosition(580, 720);

                updateBranches(score);

                // set the log flying to the left
                spriteLog.setPosition(810, 720);
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;

            }

        }

        /* Update the scene */

        if(!paused){
            // measure time
            Time dt = clock.restart();

            // subtract rom the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // size up the time bar
            timeBar.setSize(Vector2f(timeBarWidthPerSecond *
                                     timeRemaining, timeBarHeight));

            if(timeRemaining <= 0.0f){
                // pause the game
                paused = true;

                // change the message shown to the player
                messageText.setString("Out of time!");

                // reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left +
                                      textRect.width / 2.0f,
                                      textRect.top +
                                      textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                // play the out of time sound
                outOfTime.play();

            }

            // setup bee
            if(!beeActive){
                // how fast is the bee
                srand((int)time(0));
                beeSpeed = (rand() % 200) + 200;

                // how high is the bee
                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(2000, height);
                beeActive = true;

            }else{ // move the bee
                spriteBee.setPosition(
                    spriteBee.getPosition().x -
                    (beeSpeed * dt.asSeconds()),
                    spriteBee.getPosition().y);
                // has the bee reached the rigth hand edge of the screen?
                if(spriteBee.getPosition().x < -100){
                    // set it uo ready to be a whole new cloud next frame
                    beeActive = false;

                }

            }

            // manage the clouds
            // - cloud 1
            if(!cloud1Active){
                // how fast is the cloud
                srand((int)time(0) * 10);
                cloud1Speed = (rand() % 200);

                // how high is the cloud
                srand((int)time(0) % 200);

                // how high is the cloud
                srand((int)time(0) * 10);
                float height = (rand() % 150);
                spriteCloud1.setPosition(-200, height);
                cloud1Active = true;

            } else{
                spriteCloud1.setPosition(
                    spriteCloud1.getPosition().x +
                    (cloud1Speed * dt.asSeconds()),
                    spriteCloud1.getPosition().y);

                // has the cloud reached the right hand edge of the screen?
                if(spriteCloud1.getPosition().x > 1920){
                    // set it up ready to be a whole new cloud next frame
                    cloud1Active = false;
                }
            }

            // - cloud 2
            if(!cloud2Active){
                // how fast is the cloud
                srand((int)time(0) * 20);
                cloud2Speed = (rand() % 200);

                // how high is the cloud
                srand((int)time(0) % 200);

                // how high is the cloud
                srand((int)time(0) * 20);
                float height = (rand() % 300) - 150;
                spriteCloud2.setPosition(-200, height);
                cloud2Active = true;

            } else{
                spriteCloud2.setPosition(
                    spriteCloud2.getPosition().x +
                    (cloud2Speed * dt.asSeconds()),
                    spriteCloud2.getPosition().y);

                // has the cloud reached the right hand edge of the screen?
                if(spriteCloud2.getPosition().x > 1920){
                    // set it up ready to be a whole new cloud next frame
                    cloud2Active = false;
                }
            }


            // - cloud 3
            if(!cloud3Active){
                // how fast is the cloud
                srand((int)time(0) * 30);
                cloud3Speed = (rand() % 200);

                // how high is the cloud
                srand((int)time(0) % 200);

                // how high is the cloud
                srand((int)time(0) * 30);
                float height = (rand() % 450) - 150;
                spriteCloud3.setPosition(-200, height);
                cloud3Active = true;

            } else{
                spriteCloud3.setPosition(
                    spriteCloud3.getPosition().x +
                    (cloud3Speed * dt.asSeconds()),
                    spriteCloud3.getPosition().y);

                // has the cloud reached the right hand edge of the screen?
                if(spriteCloud3.getPosition().x > 1920){
                    // set it up ready to be a whole new cloud next frame
                    cloud3Active = false;
                }
            }

            // update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites
            for(int i = 0; i < NUM_BRANCHES; i++){
                float height = i * 150;

                if(branchPositions[i] == side::LEFT){
                    // move the sprite to the left side
                    branches[i].setPosition(610, height);
                    // flip the sprite round the other way
                    branches[i].setRotation(180);

                } else if(branchPositions[i] == side::RIGHT){
                    // move the sprite to the right side
                    branches[i].setPosition(1330, height);
                    // Set the sprite rotation to normal
                    branches[i].setRotation(0);

                } else{
                    // hide the branch
                    branches[i].setPosition(3000, height);

                }

            }

            // handle a flying log
            if(logActive){
                spriteLog.setPosition(
                                      spriteLog.getPosition().x +
                                      (logSpeedX * dt.asSeconds()),
                                      spriteLog.getPosition().y +
                                      (logSpeedY * dt.asSeconds())
                                     );
                // has the log reached the right hand edge?
                if(spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000){
                    // set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition(810, 720);

                }
            }

            // has the player been squished by a branch?
            if(branchPositions[5] == playerSide){
                // death
                paused = true;
                acceptInput = false;

                // draw the gravestone
                spriteRIP.setPosition(525, 760);

                // hide the player
                spritePlayer.setPosition(2000, 660);
                // change the text of tghe message
                messageText.setString("YOU DIED!");
                // center it on the screen
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left +
                                      textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f,
                                        1080 / 2.0f);

                // play the death sound
                death.play();

            }

        }// end if(!paused)

        /* Draw the scene */

        // clear everything from the last frame
        window.clear();

        // draw our game scene here
        window.draw(spriteBackground);

        // draw the clouds
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        // draw the branches
        for(int i = 0; i < NUM_BRANCHES; i++){
            window.draw(branches[i]);

        }

        // draw the tree
        window.draw(spriteTree);

        // draw the player
        window.draw(spritePlayer);

        // draw the axe
        window.draw(spriteAxe);

        // draw the flying log
        window.draw(spriteLog);

        // draw the gravestone
        window.draw(spriteRIP);

        // draw the insect
        window.draw(spriteBee);

        // draw the timebar
        window.draw(timeBar);

        // draw the score
        window.draw(scoreText);

        if(paused){
            // draw our message
            window.draw(messageText);
        }


        // show everything we just drew
        window.display();

    }

    return 0;
}

// Functions definition
void updateBranches(int seed){
    // move all the branches down one place
    for(int j = NUM_BRANCHES; j > 0; j--){
        branchPositions[j] = branchPositions[j - 1];

    }

    // spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    srand((int)time(0) + seed);
    int r = (rand() % 5);

    switch(r){
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
        case 2:
            branchPositions[0] = side::NONE;
            break;
    }


}
